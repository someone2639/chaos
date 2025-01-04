import glob, os

hvqmfiles = glob.glob("data/*.hvqm")


for i in hvqmfiles:
    result = os.path.basename(i).split(".")[0]
    print(f"HVQMVID({result})")
    pass
