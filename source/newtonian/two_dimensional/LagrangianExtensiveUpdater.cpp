#include "LagrangianExtensiveUpdater.hpp"
#include "../../misc/utils.hpp"

namespace
{
	Extensive cell_to_extensive(const ComputationalCell& cell,EquationOfState const& eos,double vol,
		TracerStickerNames const& tsn)
	{
		Extensive res;
		res.mass = vol*cell.density;
		res.momentum = res.mass*cell.velocity;
		res.energy = 0.5*ScalarProd(cell.velocity, cell.velocity)*res.mass + eos.dp2e(cell.density, cell.pressure,
			cell.tracers,tsn.tracer_names)*res.mass;
		res.tracers.resize(cell.tracers.size());
		size_t N = res.tracers.size();
		for (size_t i = 0; i < N; ++i)
			res.tracers[i] = res.mass*cell.tracers[i];
		return res;
	}
}

LagrangianExtensiveUpdater::LagrangianExtensiveUpdater(LagrangianFlux const& fc, ExtensiveUpdater const& beu,
	EquationOfState const& eos)
	:fc_(fc),beu_(beu),eos_(eos) {}

void LagrangianExtensiveUpdater::operator()
(const vector<Extensive>& fluxes,
const PhysicalGeometry& pg,
const Tessellation& tess,
const double dt,
const CacheData& cd,
const vector<ComputationalCell>& cells,
vector<Extensive>& extensives,
double time, TracerStickerNames const& tracerstickernames) const
{ 
	beu_(fluxes, pg, tess, dt, cd, cells, extensives,time, tracerstickernames);
	size_t Npoints = static_cast<size_t>(tess.GetPointNo());
	vector<double> R(Npoints, 0);
	for (size_t i = 0; i < Npoints; ++i)
		R[i] = tess.GetWidth(static_cast<int>(i));
	const vector<Edge>& edge_list = tess.getAllEdges();
	Extensive delta = dt*cd.areas[0] * fluxes[0];
	Extensive toadd(delta);
	vector<double> dV(tess.GetPointNo(), 0);
	fc_.edge_vel_.resize(edge_list.size(),0);
	fc_.ws_.resize(edge_list.size(),0);
	for (size_t i = 0; i<edge_list.size(); ++i)
	{
		const Edge& edge = edge_list[i];
		if (edge.neighbors.first < tess.GetPointNo())
			dV[static_cast<size_t>(edge.neighbors.first)] += cd.areas[i]*dt*fc_.edge_vel_[i]; 
		if (edge.neighbors.second < tess.GetPointNo())
			dV[static_cast<size_t>(edge.neighbors.second)] -= cd.areas[i]*dt*fc_.edge_vel_[i];
	}
	for (size_t i = 0; i < edge_list.size(); ++i)
	{
		double ws = fc_.ws_[i];
		Edge const& edge = tess.GetEdge(static_cast<int>(i));
		if (tess.GetOriginalIndex(edge.neighbors.first) == tess.GetOriginalIndex(edge.neighbors.second))
			continue;
		double Rcell = 0;
		if (edge.neighbors.first < static_cast<int>(Npoints))
			Rcell = R[static_cast<size_t>(edge.neighbors.first)];
		if (edge.neighbors.second < static_cast<int>(Npoints))
			Rcell = std::max(Rcell,R[static_cast<size_t>(edge.neighbors.second)]);
		if (std::abs(ws)*dt > 0.01*Rcell)
			continue;
		double L = cd.areas[i];
		if (edge.neighbors.first < tess.GetPointNo())
		{
			ReplaceExtensive(toadd, extensives[static_cast<size_t>(edge.neighbors.first)]);
			toadd *= (L*(ws)*dt / (cd.volumes[edge.neighbors.first] + dV[edge.neighbors.first]));
			extensives[static_cast<size_t>(edge.neighbors.first)] -= toadd;
		}
		if (edge.neighbors.second < tess.GetPointNo())
		{
			ReplaceExtensive(toadd, extensives[static_cast<size_t>(edge.neighbors.second)]);
			toadd *= (L*(ws)*dt / (cd.volumes[edge.neighbors.second] + dV[edge.neighbors.second]));
			extensives[static_cast<size_t>(edge.neighbors.second)] += toadd;
		}
	}
	fc_.Reset();
}
