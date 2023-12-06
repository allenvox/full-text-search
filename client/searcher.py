import ctypes
import os

libpath = os.getenv('SEARCHER_LIB_PATH')
lib = ctypes.cdll.LoadLibrary(libpath)

# Search with TextIndexAccessor
lib.initialize_text_searcher.argtypes = [ctypes.c_char_p] # index path
lib.initialize_text_searcher.restype = ctypes.c_void_p    # TextIndexAccessor

lib.finalize_text_searcher.argtypes = [ctypes.c_void_p]   # TextIndexAccessor
lib.finalize_text_searcher.restype = None                 # void

lib.search_text.argtypes = [
    ctypes.c_void_p,                 # TextIndexAccessor
    ctypes.c_char_p,                 # query
    ctypes.POINTER(ctypes.c_uint),   # document IDs
    ctypes.POINTER(ctypes.c_double), # scores
    ctypes.POINTER(ctypes.c_uint)    # results size
]
lib.search_text.restype = None # void

def initialize_text(index_path):
    return lib.initialize_text_searcher(index_path.encode('utf-8'))

def finalize_text(accessor):
    lib.finalize_text_searcher(ctypes.c_void_p(accessor))

def search_text(accessor, query):
    MAX_RESULTS = 500
    doc_ids = (ctypes.c_uint * MAX_RESULTS)()
    scores = (ctypes.c_double * MAX_RESULTS)()
    result_count = ctypes.c_uint()
    lib.search_text(ctypes.c_void_p(accessor), query.encode('utf-8'), doc_ids, scores, ctypes.byref(result_count))
    return doc_ids, scores, result_count

# Search with BinaryIndexAccessor
lib.initialize_binary_searcher.argtypes = [ctypes.c_char_p] # index.bin path
lib.initialize_binary_searcher.restype = ctypes.c_void_p    # BinaryIndexAccessor

lib.finalize_binary_searcher.argtypes = [ctypes.c_void_p]   # BinaryIndexAccessor
lib.finalize_binary_searcher.restype = None                 # void

lib.search_binary.argtypes = [
    ctypes.c_void_p,                 # BinaryIndexAccessor
    ctypes.c_char_p,                 # query
    ctypes.POINTER(ctypes.c_uint),   # document IDs
    ctypes.POINTER(ctypes.c_double), # scores
    ctypes.POINTER(ctypes.c_uint)    # results size
]
lib.search_binary.restype = None # void

def initialize_binary(index_path):
    return lib.initialize_binary_searcher(index_path.encode('utf-8'))

def finalize_binary(accessor):
    lib.finalize_binary_searcher(ctypes.c_void_p(accessor))

def search_binary(accessor, query):
    MAX_RESULTS = 500
    doc_ids = (ctypes.c_uint * MAX_RESULTS)()
    scores = (ctypes.c_double * MAX_RESULTS)()
    result_count = ctypes.c_uint()
    lib.search_binary(ctypes.c_void_p(accessor), query.encode('utf-8'), doc_ids, scores, ctypes.byref(result_count))
    return doc_ids, scores, result_count