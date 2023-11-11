import ctypes
import os
import readchar
import time

libpath = os.getenv('SEARCHER_LIB_PATH')
lib = ctypes.cdll.LoadLibrary(libpath)

lib.initialize_searcher.argtypes = [ctypes.c_char_p]
lib.initialize_searcher.restype = ctypes.c_void_p

lib.finalize_searcher.argtypes = [ctypes.c_void_p]
lib.finalize_searcher.restype = None

lib.search.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_uint)]
lib.search.restype = None

def initialize_searcher(index_path):
    return lib.initialize_searcher(index_path.encode('utf-8'))

def finalize_searcher(accessor):
    lib.finalize_searcher(ctypes.c_void_p(accessor))

def search(accessor, query):
    MAX_RESULTS = 1000
    doc_ids = (ctypes.c_uint * MAX_RESULTS)()
    scores = (ctypes.c_double * MAX_RESULTS)()
    result_count = ctypes.c_uint()
    lib.search(ctypes.c_void_p(accessor), query.encode('utf-8'), doc_ids, scores, ctypes.byref(result_count))
    return doc_ids, scores, result_count


index_path = input("Enter index path: ")
query = input("Enter searcher query: ")

accessor = initialize_searcher(index_path)

t = time.time()
ids, scores, size = search(accessor, query)
t = time.time() - t

finalize_searcher(accessor)

print("About " + str(size.value)+" results (" + str(round(t, 2)) + " seconds)")
print("score\tid\tdocument")
for i in range(size.value):
    docpath = index_path + "/index/docs/" + str(ids[i])
    with open(docpath) as f: name = f.readline().strip('\n')
    print(str(round(scores[i], 2)) + "\t" + str(ids[i]) + "\t" + name)
    if i == 5:
        print("press Q to exit or any other key to output all results")
        key = readchar.readkey()
        if key == "q":
            exit()