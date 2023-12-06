import readchar
import searcher

index_path = input("Enter index.bin path: ")
query = input("Enter searcher query: ")

accessor = searcher.initialize_binary(index_path)
ids, scores, size = searcher.search_binary(accessor, query)
searcher.finalize_binary(accessor)

print("score\toffset\tdocument")
for i in range(size.value):
    docpath = index_path + "/index/docs/" + str(ids[i])
    with open(docpath) as f: name = f.readline().strip('\n')
    print(str(round(scores[i], 2)) + "\t" + str(ids[i]) + "\t" + name)
    if i == 5:
        print("press Q to exit or any other key to output all results")
        key = readchar.readkey()
        if key == "q":
            exit()