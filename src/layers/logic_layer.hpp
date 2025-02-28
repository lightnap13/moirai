#ifndef MOIRAI_SRC_LAYERS_LOGICLAYERHPP_
#define MOIRAI_SRC_LAYERS_LOGICLAYERHPP_

#include "layers/data_layer.hpp"

#include <cstdint>

namespace moirai
{
    struct sNode;

    struct sNodePositionData
    {
        sNode*   node_array {nullptr};
        int32_t* parents_array {nullptr};
        int32_t  node_count {0};
    };

    class cLogicLayer
    {
      public:
        void update_node_geometry(sNodePositionData* node_position_data);

      private:
        void compute_node_rows(sNodePositionData* node_position_data, int32_t node_rows[MAX_NODES]);
    };
}

#endif
