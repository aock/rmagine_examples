#include <iostream>
#include <fstream>
#include <rmagine/util/StopWatch.hpp>

#include <rmagine/simulation/SimulatorEmbree.hpp>

// Generic Interface
#include <rmagine/simulation/SimulationResults.hpp>
#include <rmagine/types/Bundle.hpp>

// Predefine models
#include "rmagine_examples/models.h"
#include "rmagine_examples/helper.h"

using namespace rmagine;

int main(int argc, char** argv)
{
    std::cout << "Example CPU: O1Dn Model Simulation" << std::endl;

    if(argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [meshfile] " << std::endl;
        return 0;
    }

    // Load Map
    EmbreeMapPtr map = import_embree_map(argv[1]);
    std::cout << "Loaded file '" << argv[1] << "'" << std::endl; 

    Simulator<O1DnModel, Embree> sim;

    sim.setMap(map);

    // Create Simulator in map
    // O1DnSimulatorEmbree sim(map);

    // Define sensor model
    
    auto model = example_o1dn_model();
    sim.setModel(model);

    // Define Sensor to base transform (offset between simulated pose and scanner)
    Memory<Transform, RAM> Tsb(1);
    Tsb->setIdentity();
    sim.setTsb(Tsb);

    size_t N = 10;

    // Define poses to simulate from
    Memory<Transform, RAM> Tbm(N);
    for(size_t i=0; i<N; i++)
    {
        // for simplicity take the identity
        Tbm[i] = Tsb[0];
    }

    // define intersection attributes
    using IntAttr = Bundle<
        Ranges<RAM> 
    >;

    // simulate ranges and measure time
    StopWatch sw;
    sw();
    IntAttr res = sim.simulate<IntAttr>(Tbm);
    double el = sw();
    

    std::cout << "Simulation Statistics: " << std::endl;
    std::cout << "- Sensors: " << N << std::endl;
    std::cout << "- Rays per sensor: " << model.size() << std::endl;
    std::cout << "- Total rays: " << res.ranges.size() << std::endl;
    std::cout << "- Runtime: " << el << "s" << std::endl;

    saveRangesAsXYZ(res.ranges, model, "points_cpu_o1dn");

    return 0;
}