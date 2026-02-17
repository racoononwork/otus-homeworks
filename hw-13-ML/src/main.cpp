#include "infra_ml/models.hpp"

#include <iostream>
#include <iomanip>
#include <filesystem>

#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    po::options_description desc("Fashion MNIST Inference");
    desc.add_options()
        ("help,h", "Show help")
        ("test,t", po::value<std::string>(), "Test CSV file")
        ("model,m", po::value<std::string>(), "Model file")
        ("type", po::value<std::string>()->default_value("logreg"), "Model type")
        ("w2", po::value<std::string>(), "MLP w2 weights");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.contains("help") || !vm.contains("test") || !vm.contains("model")) {
        std::cout << desc << "\nExamples:\n";
        std::cout << "  " << argv[0] << " -t test.csv -m logreg_coef.txt -type logreg\n";
        std::cout << "  " << argv[0] << " -t test.csv -m w1.txt -w2 w2.txt -type mlp\n";
        return 1;
    }

    const std::string test_file = vm["test"].as<std::string>();
    const std::string model_file = vm["model"].as<std::string>();
    const std::string model_type = vm["type"].as<std::string>();
    const std::string w2_file = vm.contains("w2") ? vm["w2"].as<std::string>() : "";

    if (!fs::exists(test_file)) {
        std::cerr << "Test file not found: " << test_file << std::endl;
        return 1;
    }

    std::unique_ptr<Model> model;
    if (model_type == "logreg") {
        model = create_logreg_model(model_file);
    } else if (model_type == "mlp" && !w2_file.empty()) {
        model = create_mlp_model(model_file, w2_file);
    } else {
        std::cerr << "Invalid model type or missing w2 for MLP\n";
        return 1;
    }

    if (model) {
        double acc = model->compute_accuracy(test_file);
        std::cout << std::fixed << std::setprecision(3) << acc << std::endl;
    }

    return 0;
}
