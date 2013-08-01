#include <gex/comp.hpp>
#include <gex/algorithm/sampler.hpp>

    test_one<Polygon, Box, Polygon>("boxring", example_box, example_ring,
        2, 12, 1.09125);
    test_one<Polygon, Polygon, Box>("boxring2", example_ring,example_box,
        2, 12, 1.09125);

    test_one<Polygon, Box, Polygon>("boxpoly", example_box, example_polygon,
        3, 19, 0.840166);

    test_one<Polygon, Box, Polygon>("poly1", example_box,
        "POLYGON((3.4 2,4.1 3,5.3 2.6,5.4 1.2,4.9 0.8,2.9 0.7,2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2))",
        2, 12, 1.09125);

    test_one<Polygon, Box, Polygon>("clip_poly2", example_box,
        "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 2.5,5.3 2.5,5.4 1.2,4.9 0.8,2.9 0.7,2 1.3))",
        2, 12, 1.00375);

    test_one<Polygon, Box, Polygon>("clip_poly3", example_box,
        "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 2.5,4.5 2.5,4.5 1.2,4.9 0.8,2.9 0.7,2 1.3))",
        2, 12, 1.00375);

    test_one<Polygon, Box, Polygon>("clip_poly4", example_box,
        "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 2.5,4.5 2.5,4.5 2.3,5.0 2.3,5.0 2.1,4.5 2.1,4.5 1.9,4.0 1.9,4.5 1.2,4.9 0.8,2.9 0.7,2 1.3))",
        2, 16, 0.860892);

    test_one<Polygon, Box, Polygon>("clip_poly5", example_box,
        "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 2.5,4.5 1.2,2.9 0.7,2 1.3))",
        2, 11, 0.7575961);

    test_one<Polygon, Box, Polygon>("clip_poly6", example_box,
        "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.0 3.0,5.0 2.0,2.9 0.7,2 1.3))",
        2, 13, 1.0744456);

    test_one<Polygon, Box, Polygon>("clip_poly7", "Box(0 0, 3 3)",
        "POLYGON((2 2, 1 4, 2 4, 3 3, 2 2))",
        1, 4, 0.75);

