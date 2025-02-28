#include "application.hpp"

#include "debug.hpp" // TODO: Remove this once it is not in use.
#include "layers/data_layer.hpp"
#include "layers/logic_layer.hpp"
#include "layers/visual_layer.hpp"

#include <raylib/src/raylib.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <cstring> // TODO: Remove this.

namespace moirai
{
    namespace
    {
        cApplication* global_application {nullptr};
    }

    int32_t cApplication::run()
    {
        init();

        while (!WindowShouldClose())
        {
            update();
        }

        terminate();

        return EXIT_SUCCESS;
    }

    void cApplication::init()
    {
        InitWindow(800, 450, "moirai");
        SetTargetFPS(60);

        // TODO: Remove this
        // We are adding nodes manually to see if things work.
        auto add_node = [&](char* title)
        {
            int32_t node_id = _data_layer.add_node();
            sNode   node;
            strcpy(node.title, title);
            node.status = sNode::eStatus::done;
            _data_layer.set_node(node_id, &node);
            return node_id;
        };

        char    title_1[32] = "Node number 1";
        int32_t node_id_1 = add_node(title_1);

        char    title_2[64] = "I am the secon node!\nThis is second text";
        int32_t node_id_2 = add_node(title_2);

        char    title_3[64] = "Node 3";
        int32_t node_id_3 = add_node(title_3);

        char    title_4[64] = "Node namba 4";
        int32_t node_id_4 = add_node(title_4);

        _data_layer.set_parent(node_id_2, node_id_1);
        _data_layer.set_parent(node_id_3, node_id_1);
        _data_layer.set_parent(node_id_4, node_id_2);
    }

    void cApplication::update()
    {
        char window_title[32];
        std::sprintf(window_title, "morirai - %.3i FPS", GetFPS());
        SetWindowTitle(window_title);

        sNodePositionData node_position_data;
        node_position_data.node_array = _data_layer.get_node_array();
        node_position_data.parents_array = _data_layer.get_parents_array();
        node_position_data.node_count = _data_layer.get_node_count();
        _logic_layer.update_node_geometry(&node_position_data);

        if (IsKeyPressed(KEY_D) && IsKeyDown(KEY_LEFT_ALT))
        {
            std::printf("\n\n\n[DEBUG]: Printing debug info");
            debug::print_data_layer(&_data_layer);
        }

        sDrawData draw_data;
        draw_data.node_array = _data_layer.get_node_array();
        draw_data.parents_array = _data_layer.get_parents_array();
        draw_data.node_count = _data_layer.get_node_count();
        _visual_layer.draw(&draw_data);
    }

    void cApplication::terminate()
    {
        CloseWindow();
    }

    cVisualLayer* cApplication::get_visual_layer()
    {
        return &_visual_layer;
    }

    cLogicLayer* cApplication::get_logic_layer()
    {
        return &_logic_layer;
    }

    cDataLayer* cApplication::get_data_layer()
    {
        return &_data_layer;
    }

    cApplication* get_app()
    {
        return global_application;
    }

    void set_app(cApplication* app)
    {
        global_application = app;
    }
}
