import random

# Total file size in bytes (~5 MB)
size = 5 * 1024 * 1024  

# Skewed distribution
weights = [
    (ord("A"), 50),   # very common
    (ord("B"), 20),
    (ord("C"), 15),
    (ord("D"), 10),
    (ord("E"), 5)
]

# Flatten weighted list
data = []
for byte_val, weight in weights:
    data.extend([byte_val] * weight)  # byte_val is now int (0-255)

# Shuffle repeatedly until reaching target size
file_bytes = bytearray()
while len(file_bytes) < size:
    file_bytes.extend(random.choices(data, k=1000))

# Trim to exact size
file_bytes = file_bytes[:size]

with open("huffman_test.txt", "wb") as f:
    f.write(file_bytes)

print("Huffman test file created: 5 MB")
