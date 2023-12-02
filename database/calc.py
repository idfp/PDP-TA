import csv
transactions = []
drugs = []

with open("transactions.csv", "r") as t:
    csvreader = csv.DictReader(t)
    for row in csvreader:
        transactions.append(row)

with open("drugs.csv", "r") as d:
    csvreader = csv.DictReader(d)
    for row in csvreader:
        drugs.append(row)


for transaction in transactions:
    for drug in drugs:
        if(drug["name"] == transaction["drug"]):
            transaction["total"] = int(drug["price"]) * int(transaction["amount"])

with open("transactions.csv", "w") as t:
    fieldnames = ["id", "recipient", "drug", "amount", "total"]
    csvwriter = csv.DictWriter(t, fieldnames=fieldnames)
    csvwriter.writeheader()
    for transaction in transactions:
        csvwriter.writerow(transaction)