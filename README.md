# Full-Text-Search Engine
![GitHub last commit](https://img.shields.io/github/last-commit/allenvox/full-text-search)<br>
### Build
`cmake --preset release & cmake --build --preset release`<br>
### Run
Index your .csv books file: `build/bin/release/indexer-cli [--csv <path to csv>] [--index <path where to save index>]`<br>
Search through your index: `build/bin/release/searcher-cli [--index <path to index>] [--query <your search query>]`<br>
<br>
Unit tests: `ctest --preset release`<br>
