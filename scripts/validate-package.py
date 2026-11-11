"""Validate Chat Kemer LeviPack structure and the ARM64 ELF shared library."""
import json
import struct
import sys
import zipfile
from pathlib import Path

archive = Path(sys.argv[1] if len(sys.argv) > 1 else "dist/arm64-v8a/ChatKemer.levipack")
if not archive.is_file():
    raise SystemExit(f"ERROR: archive not found: {archive}")

with zipfile.ZipFile(archive) as zf:
    names = set(zf.namelist())
    required = {"manifest.json", "libChatKemer.so", "config/config.json", "config/config.schema.json"}
    missing = sorted(required - names)
    if missing:
        raise SystemExit("ERROR: missing archive entries: " + ", ".join(missing))
    if any(name.startswith("ChatKemer/") for name in names):
        raise SystemExit("ERROR: unexpected extra ChatKemer/ directory in archive root")
    manifest = json.loads(zf.read("manifest.json"))
    if manifest.get("type") != "preload-native" or manifest.get("entry") != "libChatKemer.so":
        raise SystemExit("ERROR: manifest type/entry does not match the native library")
    if zf.testzip() is not None:
        raise SystemExit("ERROR: ZIP CRC check failed")

    # ELF64 little-endian, ET_DYN, EM_AARCH64 (183).
    elf = zf.read("libChatKemer.so")[:64]
    if len(elf) < 64 or elf[:4] != b"\x7fELF":
        raise SystemExit("ERROR: libChatKemer.so is not a valid ELF file")
    if elf[4] != 2 or elf[5] != 1:
        raise SystemExit("ERROR: library must be 64-bit little-endian ELF")
    elf_type, machine = struct.unpack_from("<HH", elf, 16)
    if elf_type != 3 or machine != 183:
        raise SystemExit(f"ERROR: expected ARM64 shared object (ET_DYN/EM_AARCH64), got type={elf_type}, machine={machine}")

print(f"OK: {archive} — LeviPack structure, ZIP CRC and ARM64 ELF verified")
