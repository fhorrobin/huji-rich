#ifndef CONDITION_ACTION_SEQUENCE_HPP
#define CONDITION_ACTION_SEQUENCE_HPP 1

#include "flux_calculator_2d.hpp"
#include "../common/riemann_solver.hpp"

//! \brief First order flux calculator based on a series of conditions and actions
class ConditionActionSequence: public FluxCalculator
{
public:

  //! \brief Determines the kind of interface
  class Condition
  {
  public:

    /*! \brief Checks if an interface satisfies a condition
      \param edge Interface
      \param tess Tessellation
      \param cells Computational cells
      \return A pair of booleans. The first is whether the edge satisfies a condition, and the second is an auxiliary variable.
     */
    virtual pair<bool, bool> operator()
    (const Edge& edge,
     const Tessellation& tess,
     const vector<ComputationalCell>& cells) const = 0;

    virtual ~Condition(void);
  };

  //! \brief Action taken to calculate flux
  class Action
  {
  public:

    /*! \brief Calculates flux
      \param edge Interface between cells
      \param tess Tessellation
      \param cells Computational cells
      \param eos Equation of state
      \param aux Auxiliary variable for assymetric problems (true means the relevant cell is on the left side, false mean right)
      \return Flux
     */
    virtual Extensive operator()
    (const Edge& edge,
     const Tessellation& tess,
     const Vector2D& edge_velocity,
     const vector<ComputationalCell>& cells,
     const EquationOfState& eos,
     const bool aux) const = 0;

    virtual ~Action(void);
  };

  /*! \brief Class constructor
    \param sequence Series of condition and action action pairs
   */
  ConditionActionSequence
  (const vector<pair<const Condition*, const Action*> >& sequence);

  ~ConditionActionSequence(void);

  vector<Extensive> operator()
  (const Tessellation& tess,
   const vector<Vector2D>& edge_velocities,
   const vector<ComputationalCell>& cells,
   const vector<Extensive>& extensives,
   const CacheData& cd,
   const EquationOfState& eos,
   const double time,
   const double dt) const;

private:
   const vector<pair<const Condition*, const Action*> > sequence_;
};

//! \brief Calculates flux between two regular bulk cells
class RegularFlux: public ConditionActionSequence::Action
{
public:

  /*! \brief Class constructor
    \param rs Riemann solver
   */
  RegularFlux(const RiemannSolver& rs);

  Extensive operator()
	  (const Edge& edge,
	  const Tessellation& tess,
	  const Vector2D& edge_velocity,
	const vector<ComputationalCell>& cells,
	const EquationOfState& eos,
	const bool aux) const;

private:

  const RiemannSolver& rs_;
};

//! \brief Calculates flux assuming rigid wall boundary conditions
class RigidWallFlux: public ConditionActionSequence::Action
{
public:

  /*! \brief Class constructor
    \param rs Riemann solver
   */
  RigidWallFlux(const RiemannSolver& rs);

  Extensive operator()
  (const Edge& edge,
   const Tessellation& tess,
	  const Vector2D& edge_velocity,
	  const vector<ComputationalCell>& cells,
   const EquationOfState& eos,
   const bool aux) const;

private:
  const RiemannSolver& rs_;
};

//! \brief Estimate flux assuming free flow boundary conditions
class FreeFlowFlux: public ConditionActionSequence::Action
{
public:

  /*! \brief Class constructor
    \param rs Riemann solver
   */
  FreeFlowFlux(const RiemannSolver& rs);

  Extensive operator()
  (const Edge& edge,
   const Tessellation& tess,
	  const Vector2D& edge_velocity,
   const vector<ComputationalCell>& cells,
   const EquationOfState& eos,
   const bool aux) const;

private:
  const RiemannSolver& rs_;
};

//! \brief Checks if a certain edge is a boundary edge
class IsBoundaryEdge: public ConditionActionSequence::Condition
{
public:

  IsBoundaryEdge(void);

  pair<bool,bool> operator()
  (const Edge& edge,
   const Tessellation& tess,
   const vector<ComputationalCell>& cells) const;
};

//! \brief Check if an interface is inside the domain
class IsBulkEdge: public ConditionActionSequence::Condition
{
public:

  IsBulkEdge(void);

  pair<bool,bool> operator()
  (const Edge& edge,
   const Tessellation& tess,
   const vector<ComputationalCell>& cells) const;
};

//! \brief Determines if the interface is between a regular and a special cell
class RegularSpecialEdge: public ConditionActionSequence::Condition
{
public:

  /*! \brief Class constructor
    \param sticker_name Sticker name
   */
  RegularSpecialEdge(const string& sticker_name);

  pair<bool,bool> operator()
  (const Edge& edge,
   const Tessellation& tess,
   const vector<ComputationalCell>& cells) const;

private:
  const string sticker_name_;
};

#endif // CONDITION_ACTION_SEQUENCE_HPP
