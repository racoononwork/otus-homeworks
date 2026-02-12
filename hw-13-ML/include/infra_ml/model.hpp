#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

class Model {
public:
    virtual ~Model() = default;
    virtual int predict(const Eigen::VectorXd& features) = 0;
    virtual double compute_accuracy(const std::string& test_file) = 0;
    virtual std::string name() const = 0;
};

std::unique_ptr<Model> create_logreg_model(const std::string& path);
std::unique_ptr<Model> create_mlp_model(const std::string& w1_path, const std::string& w2_path);
