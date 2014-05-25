#include <iostream>
#include <cmath>
#include "source/newtonian/two_dimensional/hdsim2d.hpp"
#include "source/newtonian/two_dimensional/geometric_outer_boundaries/SquareBox.hpp"
#include "source/newtonian/two_dimensional/hydro_boundary_conditions/RigidWallHydro.hpp"
#include "source/tessellation/VoronoiMesh.hpp"
#include "source/newtonian/common/ideal_gas.hpp"
#include "source/newtonian/two_dimensional/interpolations/linear_gauss.hpp"
#include "source/newtonian/two_dimensional/interpolations/eos_consistent.hpp"
#include "source/newtonian/common/hllc.hpp"
#include "source/newtonian/two_dimensional/point_motions/lagrangian.hpp"
#include "source/newtonian/two_dimensional/point_motions/round_cells.hpp"
#include "source/newtonian/two_dimensional/source_terms/zero_force.hpp"
#include "source/newtonian/test_2d/square_grid.hpp"
#include "source/newtonian/two_dimensional/spatial_distributions/uniform2d.hpp"
#include "source/newtonian/two_dimensional/diagnostics.hpp"
#include "source/misc/simple_io.hpp"
#include "source/newtonian/test_2d/main_loop_2d.hpp"
#include "source/newtonian/two_dimensional/hdf5_diagnostics.hpp"

using namespace std;
using namespace simulation2d;

namespace {
  
  double azimuthal_velocity(double r)
  {
    if(r<0.2)
      return 5*r;
    else if(r>0.4)
      return 0;
    else
      return 2-5*r;
  }

  class VelocityX: public SpatialDistribution
  {
  public:
    double EvalAt(Vector2D const& r) const
    {
      if(abs(r)==0)
	return 0;
      else
	return -azimuthal_velocity(abs(r))*r.y/abs(r);
    }
  };

  class VelocityY: public SpatialDistribution
  {
  public:
    double EvalAt(Vector2D const& r) const
    {
      if(abs(r)==0)
	return 0;
      else
	return azimuthal_velocity(abs(r))*r.x/abs(r);
    }
  };

  double calc_pressure(double r)
  {
    if(r<0.2)
      return 5+(25./2.)*pow(r,2);
    else if(r>0.4)
      return 3+4*log(2.);
    else
      return 9+(25./2.)*pow(r,2)-20*r+4*log(r/0.2);
  }

  class Pressure: public SpatialDistribution
  {
  public:
    double EvalAt(Vector2D const& r) const
    {
      return calc_pressure(abs(r));
    }
  };
}

class SimData
{
public:
  
  SimData(void):
    outer_(-0.5,0.5,0.5,-0.5),
    rs_(),
    hbc_(rs_),
    tess_(),
    eos_(5./3.),
    interpm_naive_(outer_,&hbc_,true,false),
    interpm_(interpm_naive_,eos_),
    bpm_(),
    point_motion_(bpm_),
    force_(),
    sim_(offset_grid(square_grid(1,30),Vector2D(-0.5,-0.5)),
	 &tess_,
	 &interpm_,
	 Uniform2D(1),
	 Pressure(),
	 VelocityX(),
	 VelocityY(),
	 eos_,
	 rs_,
	 &point_motion_,
	 &force_,
	 &outer_,
	 &hbc_) {}

  hdsim& getSim(void)
  {
    return sim_;
  }

private:
  
  const SquareBox outer_;
  const Hllc rs_;
  const RigidWallHydro hbc_;
  VoronoiMesh tess_;
  const IdealGas eos_;
  LinearGauss interpm_naive_;
  EOSConsistent interpm_;
  Lagrangian bpm_;
  RoundCells point_motion_;
  ZeroForce force_;
  hdsim sim_;
};

namespace {

  void my_main_loop(hdsim& sim)
  {
    SafeTimeTermination term_cond(0.003, 1e6);
    WriteTime diag("time.txt");
    main_loop(sim,
	      term_cond,
	      2,
	      &diag);
  }
}

int main(void)
{
  SimData sim_data;
  hdsim& sim = sim_data.getSim();

  my_main_loop(sim);

  write_snapshot_to_hdf5(sim, "final.h5");

  return 0;
}
