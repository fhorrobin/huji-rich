#include "ConstantPrimitiveGenerator.hpp"

ConstantPrimitiveGenerator::ConstantPrimitiveGenerator(ComputationalCell const& cell) : cell_(cell){}

std::pair<ComputationalCell, ComputationalCell> ConstantPrimitiveGenerator::GetGhostGradient(Tessellation const& /*tess*/,
	vector<ComputationalCell> const& /*cells*/, vector<std::pair<ComputationalCell, ComputationalCell> > const& /*gradients*/,
	size_t /*ghost_index*/)const
{
	return std::pair<ComputationalCell, ComputationalCell>();
}

boost::container::flat_map<size_t, ComputationalCell> ConstantPrimitiveGenerator::operator() (const Tessellation& tess,
	const vector<ComputationalCell>& /*cells*/) const
{
	vector<std::pair<size_t, size_t> > outer_edges = GetOuterEdgesIndeces(tess);
	boost::container::flat_map<size_t, ComputationalCell> res;
	for (size_t i = 0; i < outer_edges.size(); ++i)
	{
		Edge const& edge = tess.GetEdge(static_cast<int>(outer_edges[i].first));
		size_t ghost_index = static_cast<size_t>(outer_edges[i].second == 1 ? edge.neighbors.first : edge.neighbors.second);
		res[ghost_index] = cell_;
	}
	return res;
}