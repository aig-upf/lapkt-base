/*
Lightweight Automated Planning Toolkit
Copyright (C) 2012
Miquel Ramirez <miquel.ramirez@rmit.edu.au>
Nir Lipovetzky <nirlipo@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __STRIPS_PROBLEM__
#define __STRIPS_PROBLEM__

#include <string>
#include <map>
#include <iosfwd>
#include <types.hxx>
#include <succ_gen.hxx>
#include <match_tree.hxx>

namespace aptk
{

	class STRIPS_Problem
	{
	public:
	
		STRIPS_Problem( std::string dom_name = "Unnamed", std::string prob_name = "Unnamed ");
		~STRIPS_Problem();

		void			set_domain_name( std::string name ) { m_domain_name = name; }
		void			set_problem_name( std::string name ) { m_problem_name = name; }
		std::string		domain_name() const { return m_domain_name; }
		std::string		problem_name() const { return m_problem_name; }

		unsigned 		num_fluents() const		{ return m_num_fluents; }
		unsigned 		num_actions() const		{ return m_num_actions; }

		void			set_num_fluents( unsigned nf ) { m_num_fluents = nf; }
		void			set_num_actions( unsigned na ) { m_num_actions = na; }

		static unsigned 	add_action( STRIPS_Problem& p, std::string signature,
						    Fluent_Vec& pre, Fluent_Vec& add, Fluent_Vec& del,
						    Conditional_Effect_Vec& ceffs, float cost = 1.0f );

		static unsigned 	add_fluent( STRIPS_Problem& p, std::string signature );

		static void		set_init( STRIPS_Problem& p, Fluent_Vec& init );
		static void		set_goal( STRIPS_Problem& p, Fluent_Vec& goal, bool createEndOp = false );

	  	
		Fluent_Ptr_Vec&		fluents() 			{ return m_fluents; }
		Action_Ptr_Vec&		actions() 			{ return m_actions; }
		const std::vector< const Fluent*>&	
					fluents() const			{ return m_const_fluents; }
		const std::vector< const Action*>&
					actions() const			{ return m_const_actions; }


		Fluent_Vec&		init()	  			{ return m_init; }
		Fluent_Vec&		goal()	  			{ return m_goal; }
		const Fluent_Vec&	init() const  			{ return m_init; }
		const Fluent_Vec&	goal() const  			{ return m_goal; }

		std::vector<const Action*>&		
					actions_adding( unsigned f )		{ return m_adding[f]; }

		std::vector< std::pair< unsigned, const Action*> >&
					ceffs_adding( unsigned f )		{ return m_ceffs_adding[f]; }

		const std::vector< std::pair< unsigned, const Action*> >&
					ceffs_adding( unsigned f ) const	{ return m_ceffs_adding[f]; }

		std::vector<const Action*>&		
					actions_deleting( unsigned f )		{ return m_deleting[f]; }

		std::vector<const Action*>&		
					actions_edeleting( unsigned f )		{ return m_edeleting[f]; }
		std::vector<const Action*>&		
					actions_requiring( unsigned f )		{ return m_requiring[f]; }
		const std::vector<const Action*>&		
					actions_adding( unsigned f ) const	{ return m_adding[f]; }
		const std::vector<const Action*>&		
					actions_deleting( unsigned f ) const	{ return m_deleting[f]; }
		const std::vector<const Action*>&		
					actions_edeleting( unsigned f ) const	{ return m_edeleting[f]; }
		const std::vector<const Action*>&		
					actions_requiring( unsigned f ) const	{ return m_requiring[f]; }

		const std::vector<const Action*>&
					empty_prec_actions() const 		{ return m_empty_precs; }

		void			applicable_actions( const State& s, std::vector<const Action* >& actions  ) const {
			m_succ_gen.retrieve_applicable(s,actions);
		}
		
		void			applicable_actions( const State& s, std::vector<int>& actions ) const {
			m_succ_gen.retrieve_applicable(s,actions);
		}
		
		void			applicable_actions_v2( const State& s, std::vector<int>& actions ) const {
			m_succ_gen_v2.retrieve_applicable(s,actions);
		}

		void			applicable_actions( const std::vector<float>& v, std::vector<const Action*>& actions ) const {
			m_succ_gen.retrieve_applicable( v, actions );
		}

		bool			is_in_init( unsigned f )	{ return m_in_init[f]; }
		bool			is_in_init( unsigned f ) const	{ return m_in_init[f]; }
		bool			is_in_goal( unsigned f )	{ return m_in_goal[f]; }
		bool			is_in_goal( unsigned f ) const	{ return m_in_goal[f]; }

		void                    print_fluent_vec(const Fluent_Vec &a);
		unsigned                end_operator() { return m_end_operator_id; }
	        unsigned                get_fluent_index(std::string signature);

		void			make_action_tables();

		void			print( std::ostream& os ) const;
		void			print_fluents( std::ostream& os ) const;
		void			print_actions( std::ostream& os ) const;
		void                    print_action( unsigned idx, std::ostream& os ) const;
		void			print_fluent_vec( std::ostream& os, const Fluent_Vec& v ) const;	
		const agnostic::Successor_Generator&
					successor_generator() const { return m_succ_gen; }

		bool			has_conditional_effects() const { return m_has_cond_effs; }
		void			notify_cond_eff_in_action() { m_has_cond_effs = true; }	

		// MRJ: Only to be used in the presence of conditional effects, as it
		// prevents the stronger inference that requires computing h^2
		void			compute_edeletes();

	protected:
	
		void			increase_num_fluents()        	{ m_num_fluents++; }
		void			increase_num_actions()        	{ m_num_actions++; }
		void			register_action_in_tables( Action* act );

	protected:

		std::string								m_domain_name;
		std::string								m_problem_name;
		unsigned		 						m_num_fluents;
		unsigned		 						m_num_actions;
		Action_Ptr_Vec		 						m_actions;
		std::vector<const Action*>						m_const_actions;
		Fluent_Ptr_Vec		 						m_fluents;
		std::vector<const Fluent*>						m_const_fluents;
		Fluent_Vec		 						m_init;
		Fluent_Vec		 						m_goal;
		Fluent_Action_Table	 						m_adding;
		Fluent_Action_Table	 						m_requiring;
		Fluent_Action_Table	 						m_deleting;
		Fluent_Action_Table	 						m_edeleting;
		std::vector<bool>	 						m_in_init;
		std::vector<bool>	 						m_in_goal;
		unsigned                 						m_end_operator_id;
	  	std::map<std::string,int> 						m_fluents_map;
		agnostic::Successor_Generator						m_succ_gen;
		agnostic::Match_Tree							m_succ_gen_v2;
		std::vector< const  Action* >   					m_empty_precs;
		std::vector< std::vector< std::pair< unsigned, const Action* > > >	m_ceffs_adding;
		bool									m_has_cond_effs;
	  };

}

#endif // STRIPS_Problem.hxx
