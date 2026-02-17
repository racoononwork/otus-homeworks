#pragma once

#include "infra_ml/model.hpp"

#include <fstream>
#include <iostream>

#include <spdlog/spdlog.h>
#include <Eigen/Dense>

class LogRegModel : public Model {
private:
    Eigen::MatrixXd weights_; // 10 x 785
    std::string name_ = "LogisticRegression";

public:
    explicit LogRegModel(const std::string &path) {
        load_model(path);
    }

    int predict(const Eigen::VectorXd &features) override {
        Eigen::VectorXd logits = weights_ * features;
        Eigen::Index max_idx;
        logits.maxCoeff(&max_idx);
        return static_cast<int>(max_idx);
    }


    double compute_accuracy(const std::string &test_file) override {
        std::ifstream file(test_file);
        if (!file.is_open()) {
            spdlog::error("Cannot open test file: {}", test_file);
            return 0.0;
        }

        int correct = 0, total = 0;
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<double> data;
            double val;
            while (iss >> val) data.push_back(val);

            if (data.size() != 785) continue;

            int true_label = static_cast<int>(data[0]);
            Eigen::VectorXd features(785);
            features(0) = 1.0; // bias
            features.tail(784) = Eigen::Map<const Eigen::VectorXd>(&data[1], 784);

            int pred = predict(features);
            if (pred == true_label) ++correct;
            ++total;
        }
        spdlog::info("LogReg accuracy: {}/{} = {}", correct, total,
                     static_cast<double>(correct) / total);
        return static_cast<double>(correct) / total;
    }

    std::string name() const override { return name_; }

private:
    void load_model(const std::string &path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            spdlog::error("Cannot open LogReg model: {}", path);
            return;
        }

        Eigen::MatrixXd raw(10, 785);
        for (int i = 0; i < 10 && file.good(); ++i) {
            for (int j = 0; j < 785; ++j) {
                file >> raw(i, j);
            }
        }
        weights_ = raw;
        spdlog::info("LogReg loaded: 10x785 from {}", path);
    }
};

std::unique_ptr<Model> create_logreg_model(const std::string &path) {
    return std::make_unique<LogRegModel>(path);
}


class MLPModel : public Model {
private:
    Eigen::MatrixXd w1_, w2_;
    std::string name_ = "MLP";

public:
    MLPModel(const std::string &w1_path, const std::string &w2_path) {
        load_model(w1_path, w2_path);
    }

    int predict(const Eigen::VectorXd &features) override {
        Eigen::VectorXd hidden = sigmoid(w1_ * (features / 255.0));
        Eigen::VectorXd output = softmax(w2_ * hidden);
        Eigen::Index max_idx;
        output.maxCoeff(&max_idx);
        return static_cast<int>(max_idx);
    }


    double compute_accuracy(const std::string &test_file) override {
        std::ifstream file(test_file);
        if (!file.is_open()) {
            spdlog::error("Cannot open test file: {}", test_file);
            return 0.0;
        }

        int correct = 0, total = 0;
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<double> data;
            double val;
            while (iss >> val) data.push_back(val);

            if (data.size() != 785) continue;

            int true_label = static_cast<int>(data[0]);
            Eigen::VectorXd features(784);
            features = Eigen::Map<const Eigen::VectorXd>(&data[1], 784);

            int pred = predict(features);
            if (pred == true_label) ++correct;
            ++total;
        }
        spdlog::info("MLP accuracy: {}/{} = {}", correct, total,
                     static_cast<double>(correct) / total);
        return static_cast<double>(correct) / total;
    }

    std::string name() const override { return name_; }

private:
    Eigen::VectorXd sigmoid(const Eigen::VectorXd &x) const {
        return (1.0 / (1.0 + (-x.array()).exp())).matrix();
    }

    Eigen::VectorXd softmax(const Eigen::VectorXd &x) const {
        Eigen::VectorXd exp_x = x.array().exp();
        return (exp_x / exp_x.sum()).matrix();
    }

    void load_model(const std::string &w1_path, const std::string &w2_path) {
        std::ifstream f1(w1_path), f2(w2_path);
        if (!f1.is_open() || !f2.is_open()) {
            spdlog::error("Cannot load MLP weights: {}, {}", w1_path, w2_path);
            return;
        }

        Eigen::MatrixXd raw_w1(784, 128);
        for (int i = 0; i < 784 && f1.good(); ++i)
            for (int j = 0; j < 128; ++j) f1 >> raw_w1(i, j);

        // Читаем w2 как 128x10
        Eigen::MatrixXd raw_w2(128, 10);
        for (int i = 0; i < 128 && f2.good(); ++i)
            for (int j = 0; j < 10; ++j) f2 >> raw_w2(i, j);

        w1_ = raw_w1.transpose();
        w2_ = raw_w2.transpose();

        spdlog::info("MLP loaded: w1=128x784, w2=10x128");
    }
};

std::unique_ptr<Model> create_mlp_model(const std::string &w1_path, const std::string &w2_path) {
    return std::make_unique<MLPModel>(w1_path, w2_path);
}


std::unique_ptr<Model> create_model(const std::string &model_type,
                                    const std::string &model_path);
