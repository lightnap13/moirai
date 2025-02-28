#include "logic_layer.hpp"

#include "layers/data_layer.hpp"
#include "node/node.hpp"

#include <cstdint>

namespace moirai
{

    void cLogicLayer::update_node_geometry(sNodePositionData* node_position_data)
    {
        int32_t node_rows[MAX_NODES];
        compute_node_rows(node_position_data, node_rows);

        int32_t nodes_per_row[MAX_NODES];
        for (uint32_t i {0U}; i < MAX_NODES; i++)
        {
            nodes_per_row[i] = 0;
        }

        for (uint32_t i {0U}; i < node_position_data->node_count; i++)
        {
            int32_t row {node_rows[i]};
            sNode*  node {&node_position_data->node_array[i]};
            node->pos_x = (nodes_per_row[row]) * (NODE_SIZE_X + HORIZONTAL_NODE_SEPARATION);
            nodes_per_row[row]++;
            node->pos_y = row * (NODE_SIZE_Y + VERTICAL_NODE_SEPARATION);
        }
    }

    void cLogicLayer::compute_node_rows(sNodePositionData* node_position_data, int32_t node_rows[MAX_NODES])
    {
        int32_t branch_nodes[MAX_NODES];

        for (uint32_t i {0U}; i < MAX_NODES; i++)
        {
            branch_nodes[i] = 0;
            node_rows[i] = -1;
        }

        for (uint32_t i {0U}; i < node_position_data->node_count; i++)
        {
            if (node_rows[i] != -1)
            {
                // We already know the position of this node, skip it.
                continue;
            }

            int32_t current_node_id {static_cast<int32_t>(i)};
            int32_t parent_id {node_position_data->parents_array[current_node_id]};

            if (parent_id == -1)
            {
                // This is a root node.
                node_rows[current_node_id] = 0;
                continue;
            }

            int32_t branch_length {0};
            branch_nodes[0] = current_node_id;

            do
            {
                // Each loop means a step up the branch, we update branch length.
                branch_length++;

                // Update parent and current.
                current_node_id = parent_id;
                parent_id = node_position_data->parents_array[current_node_id];

                // Log new current.
                branch_nodes[branch_length] = current_node_id;

            } while (parent_id != -1 && node_rows[parent_id] == -1);

            int32_t first_known_row_in_branch {0};
            if (parent_id == -1)
            {
                // New root node we did not know about.
                first_known_row_in_branch = 0;
            }
            else
            {
                // We went up the branch until finding a known node.
                first_known_row_in_branch = node_rows[parent_id] + 1;
            }

            for (uint64_t branch_index {0UL}; branch_index < branch_length; branch_index++)
            {
                const int32_t row {first_known_row_in_branch + branch_length - static_cast<int32_t>(branch_index)};
                node_rows[branch_nodes[branch_index]] = row;
            }
        }
    }
}
