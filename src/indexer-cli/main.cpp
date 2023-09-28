#include <indexer/indexer.hpp>

#include <iostream>

IndexText get_user_input(const IndexText &message) {
    std::cout << message << '\n' << "> ";
    IndexText input;
    getline(std::cin, input);
    return input;
}

int main() {
    IndexBuilder indexbuilder({"the"}, 3, 6);
    indexbuilder.add_document(199903, "The Matrix");
    indexbuilder.add_document(200305, "The Matrix Reloaded");
    indexbuilder.add_document(200311, "The Matrix Revolution");
    Index index = indexbuilder.index();

    std::string out_path = get_user_input("Enter output path for index:");
    const std::filesystem::path path = out_path;

    TextIndexWriter writer;
    writer.write(path, index);

    std::cout << std::endl;
    return 0;
}