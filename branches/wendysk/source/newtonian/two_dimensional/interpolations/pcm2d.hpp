/*! \brief A 2D piecewise contant interpolation method
  \author Almog Yalinewich
 */

#ifndef PCM2D_HPP
#define PCM2D_HPP 1

#include "../spatial_reconstruction.hpp"

//! \brief Piecewise constant method
class PCM2D: public SpatialReconstruction
{
public:

  void Prepare(Tessellation const* tessellation,
	       vector<Primitive> const& cells,
	       double dt,vector<bool> const& mask,double time);

  Primitive Interpolate(Tessellation const* /*tessellation*/,
			vector<Primitive> const& cells,
			double /*dt*/,Edge const& edge, int side,
			InterpolationType /*interptype*/) const;
  
  bool WasSlopeLimited(int index)const;
};

#endif // PCM2D_HPP
