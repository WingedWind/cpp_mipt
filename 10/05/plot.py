import csv
import matplotlib.pyplot as plt

with open("hash_collisions.csv") as f:
    reader = csv.DictReader(f)
    rows = list(reader)

counts = [int(r["count"]) for r in rows]
hashes = [k for k in rows[0].keys() if k != "count"]

for name in hashes:
    values = [int(r[name]) for r in rows]
    plt.plot(counts, values, marker="o", label=name)

plt.xlabel("Number of hashed strings")
plt.ylabel("Number of collisions")
plt.title("Hash function collision comparison")
plt.legend()
plt.tight_layout()
plt.savefig("hash_collisions.png", dpi=150)
plt.show()

