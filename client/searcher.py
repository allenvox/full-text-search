import ctypes
import os

libpath = os.getenv('SEARCHER_LIB_PATH')
lib = ctypes.cdll.LoadLibrary(libpath)

lib.initialize_searcher.argtypes = [ctypes.c_char_p] # index path
lib.initialize_searcher.restype = ctypes.c_void_p    # IndexAccessor

lib.finalize_searcher.argtypes = [ctypes.c_void_p]   # IndexAccessor
lib.finalize_searcher.restype = None                 # void

lib.search.argtypes = [
    ctypes.c_void_p,                 # IndexAccessor
    ctypes.c_char_p,                 # query
    ctypes.POINTER(ctypes.c_uint),   # document IDs
    ctypes.POINTER(ctypes.c_double), # scores
    ctypes.POINTER(ctypes.c_uint)    # results size
]
lib.search.restype = None # void

def initialize(index_path):
    return lib.initialize_searcher(index_path.encode('utf-8'))

def finalize(accessor):
    lib.finalize_searcher(ctypes.c_void_p(accessor))

def search(accessor, query):
    MAX_RESULTS = 500
    doc_ids = (ctypes.c_uint * MAX_RESULTS)()
    scores = (ctypes.c_double * MAX_RESULTS)()
    result_count = ctypes.c_uint()
    lib.search(ctypes.c_void_p(accessor), query.encode('utf-8'), doc_ids, scores, ctypes.byref(result_count))
    return doc_ids, scores, result_count