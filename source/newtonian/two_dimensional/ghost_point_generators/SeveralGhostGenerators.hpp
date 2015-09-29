/*! \file SeveralGhostGenerators.hpp
\brief Class for creating computationalcells of ghost points from several different methods
\author Elad Steinberg
*/

#ifndef SEVERAL_GHOST_GENERATOR_HPP
#define SEVERAL_GHOST_GENERATOR_HPP 1

#include "../GhostPointGenerator.hpp"

/*! \brief Class for chhosing which ghost generator to use
\author Elad Steinberg
*/
class GhostCriteria
{
public:
	/*!
	\brief Chooses the ghost generator
	\param tess The tessellation
	\param index The index of the mesh point to calculate for
	\return The index in the ghost generator vector to choose from
	*/
	virtual size_t GhostChoose(Tessellation const& tess, int index)const = 0;

	virtual ~GhostCriteria(void){};
};

/*! \brief Class for creating computationalcells of ghost points from several different methods
\author Elad Steinberg
*/
class SeveralGhostGenerators : public GhostPointGenerator
{
private:
	vector<GhostPointGenerator*> ghosts_;
	GhostCriteria const& ghost_chooser_;
public:

	SeveralGhostGenerators(vector<GhostPointGenerator*> ghosts,GhostCriteria const& ghostchooser);

	boost::container::flat_map<size_t, ComputationalCell> operator() (const Tessellation& tess,
		const vector<ComputationalCell>& cells) const;

	std::pair<ComputationalCell, ComputationalCell> GetGhostGradient(const Tessellation& tess,
		const vector<ComputationalCell>& cells, const vector<std::pair<ComputationalCell, ComputationalCell> >& gradients,
		size_t ghost_index) const;

	Vector2D GetGhostVelocity(const Tessellation& tess, const vector<ComputationalCell>& cells,
		vector<Vector2D> const& point_veolcities, size_t ghost_index, Edge const& edge)const;
};

#endif // SEVERAL_GHOST_GENERATOR_HPP