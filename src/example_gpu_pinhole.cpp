#include <iostream>
#include <fstream>
#include <rmagine/simulation/SimulatorOptix.hpp>
#include <rmagine/util/StopWatch.hpp>
#include <rmagine/math/math.h>

#include "rmagine_examples/models.h"
#include "rmagine_examples/helper.h"

using namespace rmagine;

int main(int argc, char** argv)
{
    std::cout << "Example GPU: Simulate pinhole model." << std::endl;

    if(argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [meshfile] " << std::endl;
        return 0;
    }

    // Load Map
    OptixMapPtr map = import_optix_map(argv[1]);
    std::cout << "Loaded file '" << argv[1] << "'" << std::endl;

    size_t N = 10;

    // Create Simulator in map
    Simulator<PinholeModel, Optix> sim_pinhole(map);

    // Define sensor model
    Memory<PinholeModel, RAM> model = example_pinhole_model();
    sim_pinhole.setModel(model);

    // Define Sensor to base transform (offset between simulated pose and scanner)
    Memory<Transform, RAM> Tsb(1);
    Tsb->setIdentity();
    sim_pinhole.setTsb(Tsb);

    // Define poses to simulate from
    Memory<Transform, RAM> Tbm_(N);
    for(size_t i=0; i<N; i++)
    {
        // for simplicity take the identity
        Tbm_[i] = Tsb[0];
    }

    // upload to gpu
    Memory<Transform, VRAM_CUDA> Tbm;
    Tbm = Tbm_;

    // defining return attributes
    using IntAttr = Bundle<
        Ranges<VRAM_CUDA> 
    >;

    // simulate ranges and measure time
    StopWatch sw;
    double el;
    
    Memory<float, RAM> ranges;
    
    sw();
    IntAttr res = sim_pinhole.simulate<IntAttr>(Tbm);
    el = sw();

    // Download
    ranges = res.ranges;

    std::cout << "Simulation Statistics: " << std::endl;
    std::cout << "- Sensors: " << N << std::endl;
    std::cout << "- Rays per sensor: " << model->size() << std::endl;
    std::cout << "- Total rays: " << ranges.size() << std::endl;
    std::cout << "- Runtime: " << el << "s" << std::endl;

    saveRangesAsXYZ(ranges, *model, "points_gpu_pinhole");

    return 0;
}