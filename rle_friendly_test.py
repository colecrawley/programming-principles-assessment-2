# Total file size in bytes (~5 MB)
size = 5 * 1024 * 1024  

# Long runs
runs = [
    (b"\x00", 50000),
    (b"\xFF", 70000),
    (b"\xAA", 30000),
    (b"\x55", 100000)
]

file_bytes = bytearray()
while len(file_bytes) < size:
    for byte, run_len in runs:
        file_bytes.extend(byte * run_len)
        if len(file_bytes) >= size:
            break

# Trim to exact size
file_bytes = file_bytes[:size]

with open("rle_test.bin", "wb") as f:
    f.write(file_bytes)

print("RLE test file created: 5 MB")
