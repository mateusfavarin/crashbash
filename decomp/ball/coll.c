#include <math.h>
#include "coll.h"

PillarDatabase pillarDatabase[NUM_PILLARS] = {
    /* Crashball */
    [0] = {
        .pos = {
            [0] = {
                .x = FP(8), .z = FP(8), .radius = FP(2)
            },
            [1] = {
                .x = FP(8), .z = FP(-8), .radius = FP(2)
            },
            [2] = {
                .x = FP(-8), .z = FP(8), .radius = FP(2)
            },
            [3] = {
                .x = FP(-8), .z = FP(-8), .radius = FP(2)
            },
        },
        .maybeUnused = {
            [0] = {
                .x = FP(0), .z = FP(-16), .radius = FP(2)
            },
            [1] = {
                .x = FP(0), .z = FP(16), .radius = FP(2)
            },
            [2] = {
                .x = FP(16), .z = FP(0), .radius = FP(2)
            },
            [3] = {
                .x = FP(-16), .z = FP(0), .radius = FP(2)
            },
        },
        .ballSpawnPos = {
            [0] = {
                .x = FP(-8), .y = FP(-0.125), .z = FP(-8)
            },
            [1] = {
                .x = FP(8), .y = FP(-0.125), .z = FP(-8)
            },
            [2] = {
                .x = FP(-8), .y = FP(-0.125), .z = FP(8)
            },
            [3] = {
                .x = FP(8), .y = FP(-0.125), .z = FP(8)
            },
        }
    },
    /* Beach Ball */
    [1] = {
        .pos = {
            [0] = {
                .x = FP(8), .z = FP(8), .radius = FP(2)
            },
            [1] = {
                .x = FP(8), .z = FP(-8), .radius = FP(2)
            },
            [2] = {
                .x = FP(-8), .z = FP(8), .radius = FP(2)
            },
            [3] = {
                .x = FP(-8), .z = FP(-8), .radius = FP(2)
            },
        },
        .maybeUnused = {
            [0] = {
                .x = FP(0), .z = FP(-16), .radius = FP(2)
            },
            [1] = {
                .x = FP(0), .z = FP(16), .radius = FP(2)
            },
            [2] = {
                .x = FP(16), .z = FP(0), .radius = FP(2)
            },
            [3] = {
                .x = FP(-16), .z = FP(0), .radius = FP(2)
            },
        },
        .ballSpawnPos = {
            [0] = {
                .x = FP(-8), .y = FP(-0.125), .z = FP(-8)
            },
            [1] = {
                .x = FP(8), .y = FP(-0.125), .z = FP(-8)
            },
            [2] = {
                .x = FP(-8), .y = FP(-0.125), .z = FP(8)
            },
            [3] = {
                .x = FP(8), .y = FP(-0.125), .z = FP(8)
            },
        }
    },
    /* N. Ballism */
    [2] = {
        .pos = {
            [0] = {
                .x = FP(8.315), .z = FP(8.315), .radius = FP(1.625)
            },
            [1] = {
                .x = FP(8.315), .z = FP(-8.315), .radius = FP(1.625)
            },
            [2] = {
                .x = FP(-8.315), .z = FP(8.315), .radius = FP(1.625)
            },
            [3] = {
                .x = FP(-8.315), .z = FP(-8.315), .radius = FP(1.625)
            },
        },
        .maybeUnused = {
            [0] = {
                .x = FP(0), .z = FP(-16), .radius = FP(2)
            },
            [1] = {
                .x = FP(0), .z = FP(16), .radius = FP(2)
            },
            [2] = {
                .x = FP(16), .z = FP(0), .radius = FP(2)
            },
            [3] = {
                .x = FP(-16), .z = FP(0), .radius = FP(2)
            },
        },
        .ballSpawnPos = {
            [0] = {
                .x = FP(-9), .y = FP(-5), .z = FP(-9)
            },
            [1] = {
                .x = FP(9), .y = FP(-5), .z = FP(-9)
            },
            [2] = {
                .x = FP(-9), .y = FP(-5), .z = FP(9)
            },
            [3] = {
                .x = FP(9), .y = FP(-5), .z = FP(9)
            },
        }
    },
    /* Sky Balls */
    [3] = {
        .pos = {
            [0] = {
                .x = FP(7.3025), .z = FP(7.3025), .radius = FP(1.815)
            },
            [1] = {
                .x = FP(7.3025), .z = FP(-7.3025), .radius = FP(1.815)
            },
            [2] = {
                .x = FP(-7.3025), .z = FP(7.3025), .radius = FP(1.815)
            },
            [3] = {
                .x = FP(-7.3025), .z = FP(-7.3025), .radius = FP(1.815)
            },
        },
        .maybeUnused = {
            [0] = {
                .x = FP(0), .z = FP(-16), .radius = FP(1.7)
            },
            [1] = {
                .x = FP(0), .z = FP(16), .radius = FP(1.7)
            },
            [2] = {
                .x = FP(16), .z = FP(0), .radius = FP(1.7)
            },
            [3] = {
                .x = FP(-16), .z = FP(0), .radius = FP(1.7)
            },
        },
        .ballSpawnPos = {
            [0] = {
                .x = FP(-8), .y = FP(-8), .z = FP(-8)
            },
            [1] = {
                .x = FP(8), .y = FP(-8), .z = FP(-8)
            },
            [2] = {
                .x = FP(-8), .y = FP(-8), .z = FP(8)
            },
            [3] = {
                .x = FP(8), .y = FP(-8), .z = FP(8)
            },
        }
    },
};