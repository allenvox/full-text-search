#include <indexer/indexer.hpp>

#include <iostream>

IndexText get_user_input(const IndexText &message) {
    std::cout << message << '\n' << "> ";
    IndexText input;
    getline(std::cin, input);
    return input;
}

int main() {
    IndexBuilder indexbuilder({"and", "the", "of", "on"}, 3, 6);
    indexbuilder.add_document(199903,
                              "Harry Potter and the Philosopher's Stone");
    indexbuilder.add_document(199925, "Harry Potter and the Deathly Hallows");
    indexbuilder.add_document(199937,
                              "Harry Potter and the Chamber of Secrets");
    indexbuilder.add_document(199951, "The Shining");
    indexbuilder.add_document(199964, "Infinite Jest");
    indexbuilder.add_document(200101, "The Clockwork Orange");
    indexbuilder.add_document(200305, "All Quiet on the Western Front");
    const Index index = indexbuilder.index();

    std::string out_path = get_user_input("Enter output path for index:");
    if (out_path.empty()) {
        out_path = "build";
    }
    const std::filesystem::path path = out_path;

    const TextIndexWriter writer;
    writer.write(path, index);

    std::cout << std::endl;
    return 0;
}