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

#ifndef __SIW__
#define __SIW__

#include <aptk/search_prob.hxx>
#include <aptk/resources_control.hxx>
#include <aptk/closed_list.hxx>
#include <aptk/serialized_search.hxx>
#include <aptk/iw.hxx>
#include <h_1.hxx>
#include <vector>
#include <algorithm>
#include <iostream>

namespace aptk {

namespace search {


template < typename Search_Model >
class SIW : public Serialized_Search<Search_Model, brfs::IW<Search_Model, aptk::agnostic::Novelty<Search_Model>>, aptk::search::brfs::Node< aptk::State >> {

public:

	typedef  	 aptk::search::brfs::Node< aptk::State >					Search_Node;

	SIW( const Search_Model& search_problem ) 
		: Serialized_Search<Search_Model, brfs::IW<Search_Model, aptk::agnostic::Novelty<Search_Model>>, Search_Node>( search_problem ), m_pruned_sum_B_count(0), m_sum_B_count(0), m_max_B_count(0), m_iw_calls(0) {	   	
	}

	virtual ~SIW() {
	}

	virtual bool	find_solution( float& cost, std::vector<Action_Idx>& plan ) {
		
		Search_Node* end = NULL;
		State* new_init_state = NULL;
		this->m_goals_achieved.clear();
		this->m_goal_candidates.clear();
		this->m_goal_candidates.insert( this->m_goal_candidates.begin(), 
					  this->problem().task().goal().begin(), this->problem().task().goal().end() );

		do{
			
			std::cout << std::endl << "{" << this->m_goal_candidates.size() << "/" << this->m_goals_achieved.size() << "}:IW(" << this->bound() << ") -> ";
			end = this->do_search();		
			m_pruned_sum_B_count += this->pruned_by_bound();

			if ( end == NULL ) {

				/**
				 * If no partial plan to achieve any goal is  found,
				 * throw IW(b+1) from same root node
				 *
				 * If no state has been pruned by bound, then IW is in a dead-end,
				 * return NO-PLAN
				 */
				if( this->pruned_by_bound() == 0)
					return false;
				
				new_init_state = new State( this->problem().task() );
				new_init_state->set( this->m_root->state()->fluent_vec() );
				new_init_state->update_hash();				

				this->set_bound( this->bound()+1 );				
				this->start( new_init_state );				

			}
			else{

				/**
				 * If a partial plan extending the achieved goals set is found,
				 * IW(1) is thrown from end_state
				 */

				m_max_B_count = m_max_B_count < this->bound() ? this->bound() : m_max_B_count; 
				m_sum_B_count += this->bound();
				m_iw_calls++;

				std::vector<Action_Idx> partial_plan;
				this->extract_plan( this->m_root, end, partial_plan, cost );	
				plan.insert( plan.end(), partial_plan.begin(), partial_plan.end() );			
				
				new_init_state = new State( this->problem().task() );
				new_init_state->set( end->state()->fluent_vec() );
				new_init_state->update_hash();
				
				this->set_bound( 1 );
				this->start( new_init_state );
			}


		}while( !this->problem().goal( *new_init_state ) );


		
		return true;
	}

	unsigned		sum_pruned_by_bound() const		{ return m_pruned_sum_B_count; }
	float                   avg_B() const { return (float)(m_sum_B_count) / m_iw_calls; }
	unsigned                max_B() const { return m_max_B_count; }
	
protected:	       
	unsigned		m_pruned_sum_B_count;
	unsigned		m_sum_B_count;
	unsigned		m_max_B_count;
	unsigned		m_iw_calls;
};

}
		
}



#endif // siw.hxx