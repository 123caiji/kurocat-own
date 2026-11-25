#!/usr/bin/env node
// tools/img.mjs — 零依赖：BMP → PNG / GIF（GIF 带透明色）
import fs from 'node:fs';
import zlib from 'node:zlib';

function readBmp(file) {
  const b = fs.readFileSync(file);
  const w = b.readInt32LE(18), h = b.readInt32LE(22), bpp = b.readUInt16LE(28), off = b.readUInt32LE(10);
  if (bpp !== 24) throw new Error('need 24bpp bmp');
  const rb = Math.floor((24 * w + 31) / 32) * 4;
  const px = Buffer.alloc(w * h * 3);
  for (let y = 0; y < h; y++) {
    const src = off + (h - 1 - y) * rb;
    for (let x = 0; x < w; x++) {
      const s = src + x * 3, d = (y * w + x) * 3;
      px[d] = b[s + 2]; px[d + 1] = b[s + 1]; px[d + 2] = b[s];
    }
  }
  return { w, h, px };
}

function writePng(file, w, h, px) {
  const raw = Buffer.alloc((w * 3 + 1) * h);
  for (let y = 0; y < h; y++) {
    raw[y * (w * 3 + 1)] = 0;
    px.copy(raw, y * (w * 3 + 1) + 1, y * w * 3, (y + 1) * w * 3);
  }
  const chunk = (type, data) => {
    const len = Buffer.alloc(4); len.writeUInt32BE(data.length);
    const t = Buffer.from(type, 'ascii');
    const crc = Buffer.alloc(4);
    crc.writeUInt32BE(zlib.crc32(Buffer.concat([t, data])) >>> 0);
    return Buffer.concat([len, t, data, crc]);
  };
  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(w, 0); ihdr.writeUInt32BE(h, 4);
  ihdr[8] = 8; ihdr[9] = 2; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
  fs.writeFileSync(file, Buffer.concat([
    Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]),
    chunk('IHDR', ihdr), chunk('IDAT', zlib.deflateSync(raw, { level: 9 })), chunk('IEND', Buffer.alloc(0)),
  ]));
}

// ---- GIF ----
function quantize(px, n) {
  // 固定 6x6x6 调色板 + 近黑/近白补充，够用且无需复杂算法
  const colors = [];
  for (let r = 0; r < 6; r++) for (let g = 0; g < 6; g++) for (let b = 0; b < 6; b++) colors.push([r * 51, g * 51, b * 51]);
  colors.push([0,0,0],[255,255,255],[16,16,18],[240,240,244]);
  return colors.slice(0, n);
}
function nearest(c, pal) {
  let best = 0, bd = 1e9;
  for (let i = 0; i < pal.length; i++) {
    const d = (c[0]-pal[i][0])**2 + (c[1]-pal[i][1])**2 + (c[2]-pal[i][2])**2;
    if (d < bd) { bd = d; best = i; }
  }
  return best;
}
function lzwEncode(indices, minCode) {
  const clear = 1 << minCode, eoi = clear + 1;
  let codeSize = minCode + 1, next = eoi + 1;
  const dict = new Map();
  const out = [];
  let cur = 0, curBits = 0;
  const emit = (code) => {
    cur |= code << curBits; curBits += codeSize;
    while (curBits >= 8) { out.push(cur & 0xff); cur >>= 8; curBits -= 8; }
  };
  emit(clear);
  let prefix = indices[0];
  for (let i = 1; i < indices.length; i++) {
    const k = indices[i];
    const key = prefix * 4096 + k;
    const found = dict.get(key);
    if (found !== undefined) { prefix = found; continue; }
    emit(prefix);
    if (next < 4096) {
      dict.set(key, next++);
      if (next > (1 << codeSize) && codeSize < 12) codeSize++;
    } else {
      emit(clear); dict.clear(); codeSize = minCode + 1; next = eoi + 1;
    }
    prefix = k;
  }
  emit(prefix);
  emit(eoi);
  if (curBits > 0) out.push(cur & 0xff);
  return Buffer.from(out);
}
function writeGif(file, frames, w, h, delayCs, transparentIdx) {
  const parts = [];
  const u16 = (v) => { const b = Buffer.alloc(2); b.writeUInt16LE(v); return b; };
  parts.push(Buffer.from('GIF89a', 'ascii'), u16(w), u16(h));
  parts.push(Buffer.from([0xf7, 0, 0]));           // GCT 256 色
  const pal = frames[0].pal;
  for (let i = 0; i < 256; i++) {
    const c = pal[i] || [0, 0, 0];
    parts.push(Buffer.from([c[0], c[1], c[2]]));
  }
  if (transparentIdx >= 0) parts.push(Buffer.from([0x21, 0xf9, 4, 0x01, 0, 0, transparentIdx, 0]));
  parts.push(Buffer.from([0x21, 0xff, 11]), Buffer.from('NETSCAPE2.0', 'ascii'), Buffer.from([3, 1, 0, 0, 0]));
  for (const f of frames) {
    parts.push(Buffer.from([0x21, 0xf9, 4, 0x00]), u16(delayCs), Buffer.from([transparentIdx >= 0 ? transparentIdx : 0, 0]));
    parts.push(Buffer.from([0x2c]), u16(0), u16(0), u16(w), u16(h), Buffer.from([0x00]));
    const mcs = 8;
    parts.push(Buffer.from([mcs]));
    const data = lzwEncode(f.indices, mcs);
    for (let i = 0; i < data.length; i += 255) parts.push(Buffer.from([Math.min(255, data.length - i)]), data.subarray(i, i + 255));
    parts.push(Buffer.from([0x00]));
  }
  parts.push(Buffer.from([0x3b]));
  fs.writeFileSync(file, Buffer.concat(parts));
}

const [cmd, ...args] = process.argv.slice(2);
if (cmd === 'png') {
  const { w, h, px } = readBmp(args[0]);
  writePng(args[1], w, h, px);
  console.log('png ok', w, 'x', h);
} else if (cmd === 'gif') {
  // gif <outDir> <out.gif> <delayCs> [bgColor=white|dark]
  const dir = args[0], out = args[1], delay = Number(args[2] || 8), bg = args[3] || 'white';
  const files = fs.readdirSync(dir).filter((f) => f.endsWith('.bmp')).sort();
  const pal = quantize(null, 256);
  let w = 0, h = 0;
  const frames = [];
  for (const f of files) {
    const img = readBmp(dir + '/' + f);
    w = img.w; h = img.h;
    const n = w * h;
    const indices = new Uint8Array(n);
    for (let i = 0; i < n; i++) {
      let r = img.px[i*3], g = img.px[i*3+1], b = img.px[i*3+2];
      // 把近白背景统一成纯白，便于当透明色键
      if (r > 235 && g > 235 && b > 235) { r = 255; g = 255; b = 255; }
      indices[i] = nearest([r, g, b], pal);
    }
    frames.push({ indices, pal });
  }
  // 透明色 = 白色索引
  let whiteIdx = 255;
  for (let i = 0; i < pal.length; i++) if (pal[i][0] > 250 && pal[i][1] > 250 && pal[i][2] > 250) { whiteIdx = i; break; }
  writeGif(out, frames, w, h, delay, bg === 'transparent' ? whiteIdx : -1);
  console.log('gif ok', files.length, 'frames', w + 'x' + h);
}
