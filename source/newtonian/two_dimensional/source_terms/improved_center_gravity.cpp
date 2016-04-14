#include "improved_center_gravity.hpp"

ImprovedCenterGravity::ImprovedCenterGravity
(double M,double Rmin,const Vector2D& center):
  M_(M),Rmin_(Rmin),center_(center){}

Vector2D ImprovedCenterGravity::operator()
  (const Tessellation& tess,
   const vector<ComputationalCell>& /*cells*/,
   const vector<Extensive>& /*fluxes*/,
   const double /*time*/,
   const int point,
   TracerStickerNames const& /*tracerstickernames*/) const
{
  const Vector2D pos = tess.GetCellCM(point)-center_;
  const double r = abs(pos);
  return (-1)*pos*M_/(r*r*r+Rmin_*Rmin_*Rmin_);
}

Vector2D const& ImprovedCenterGravity::get_center(void) const
{
  return center_;
}
