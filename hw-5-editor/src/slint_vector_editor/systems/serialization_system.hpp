#pragma once

#include <iostream>
#include <fcntl.h> 
#include <unistd.h>

#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <kj/io.h>

#include <slint_vector_editor/ecs/registry.hpp>
#include <slint_vector_editor/components/components.hpp>
#include <slint_vector_editor/serialization/vector_editor.capnp.h>

class SerializationSystem {
public:
    explicit SerializationSystem(ecs::Registry& reg) : registry(reg) {}

    void save(const std::string& path) {
        ::capnp::MallocMessageBuilder message;
        Document::Builder doc = message.initRoot<Document>();
        
        auto entities = registry.view();
        auto shapesList = doc.initShapes(entities.size());

        for (size_t i = 0; i < entities.size(); ++i) {
            auto shapeBuilder = shapesList[i];
            const auto& entity = entities[i];
            
            shapeBuilder.setId(entity.id);
            shapeBuilder.setX(entity.transform.x);
            shapeBuilder.setY(entity.transform.y);
            shapeBuilder.setWidth(entity.transform.width);
            shapeBuilder.setHeight(entity.transform.height);
            
            if (entity.shape.type == ShapeType::Rectangle)
                shapeBuilder.setType(Shape::Type::RECT);
            else
                shapeBuilder.setType(Shape::Type::LINE);
        }

        int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            std::cerr << "Error: Cannot open file " << path << " for writing." << std::endl;
            return;
        }

        try {
            kj::FdOutputStream fdStream(fd);
            
            kj::BufferedOutputStreamWrapper bufferedStream(fdStream);
            
            capnp::writePackedMessage(bufferedStream, message);
            
            std::cout << "Saved to " << path << std::endl;
        } catch (kj::Exception& e) {
            std::cerr << "Cap'n Proto saving error: " << e.getDescription().cStr() << std::endl;
        }

        close(fd);
    }

    void load(const std::string& path) {
        int fd = open(path.c_str(), O_RDONLY);
        if (fd < 0) {
            std::cerr << "Error: Cannot open file " << path << " for reading." << std::endl;
            return;
        }
        
        try {
            kj::FdInputStream fdStream(fd);
            
            kj::BufferedInputStreamWrapper bufferedStream(fdStream);

            capnp::PackedMessageReader message(bufferedStream);
            Document::Reader doc = message.getRoot<Document>();
            
            registry.clear();

            for (auto shapeReader : doc.getShapes()) {
                ShapeType type = (shapeReader.getType() == Shape::Type::RECT) 
                                 ? ShapeType::Rectangle 
                                 : ShapeType::Line;
                
                registry.createEntity(
                    type,
                    shapeReader.getX(),
                    shapeReader.getY(),
                    shapeReader.getWidth(),
                    shapeReader.getHeight()
                );
            }
            std::cout << "Loaded from " << path << std::endl;
        } catch (kj::Exception& e) {
            std::cerr << "Cap'n Proto loading error: " << e.getDescription().cStr() << std::endl;
        }
        
        close(fd);
    }

private:
    ecs::Registry& registry;
};