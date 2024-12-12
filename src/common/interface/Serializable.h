#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace Metal {
    struct Serializable {
        virtual ~Serializable() = default;

        void serialize(const std::string &pathToFile) {
            std::ofstream ofs(pathToFile, std::ios::binary);
            if (!ofs) {
                throw std::runtime_error("Could not open file " + pathToFile);
            }
            writeFields(ofs); // EXAMPLE ofs.write(reinterpret_cast<char*>(&value), sizeof(value)); // Write float
            ofs.close();
        }

        void deserialize(const std::string &pathToFile) {
            std::ifstream ifs(pathToFile, std::ios::binary);
            readFields(ifs); // EXAMPLE ifs.read(reinterpret_cast<char *>(&id), sizeof(id)); // Read integer
            ifs.close();
        }

    protected:
        virtual void writeFields(const std::ofstream &ofs) {
            throw std::runtime_error("Not implemented");
        }

        virtual void readFields(const std::ifstream &ifs) {
            throw std::runtime_error("Not implemented");
        }
    };
}
#endif //SERIALIZABLE_H
