/*
Lightweight Automated Planning Toolkit (LAPKT)
Copyright (C) 2015

<contributors>
Miquel Ramirez <miquel.ramirez@gmail.com>
</contributors>

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

#pragma once

#include <vector>
#include <unordered_set>
#include <aptk2/heuristics/interfaces/novelty_evaluator.hxx>
#include <aptk2/heuristics/novelty/tuples.hxx>
#include <cassert>
#include <boost/functional/hash.hpp>

namespace aptk {

	template < typename StateType >
	class FiniteDomainNoveltyEvaluator : public NoveltyEvaluator< StateType > {
	public:
		typedef std::vector< std::unordered_set< ValuesTuple, ValuesTuple::Hasher > >	NoveltyTables;
		typedef NoveltyEvaluator< StateType >		BaseClass;

		FiniteDomainNoveltyEvaluator()
			: BaseClass() {
		}
		virtual ~FiniteDomainNoveltyEvaluator() { }

		virtual unsigned	initialize( const StateType& s ) {
			return 1;
		}

		//! A micro-optimization to deal faster with the analysis of width-1 tuples
		unsigned evaluate_width_1_tuples() {
			unsigned current_novelty = _varnames.size() + 1;
			for (unsigned i = 0; i < _varnames.size(); ++i) {
				auto res = _width_1_tuples.insert(std::make_pair(_varnames[i], _valuation[i]));
				if (!res.second) continue; // This tuple was already accounted for
				current_novelty = 1; // It wasn't
			}
			return current_novelty;
		}
		
		virtual unsigned evaluate( const StateType& s ) {
			assert( _tables.size() > 0 );

			s.get_valuation( _varnames, _valuation );

			unsigned current_novelty = evaluate_width_1_tuples();

			for (unsigned min_novelty = 2; min_novelty <= BaseClass::max_novelty(); ++min_novelty) {

				ValuesTupleIterator it( _varnames, _valuation, min_novelty );
				ValuesTuple t( min_novelty );
				bool updated_tables = false;
				while ( it.next(t) ) {
					auto result = _tables[min_novelty].insert( t );
					updated_tables |= result.second;
				}
				
				if ( updated_tables && min_novelty < current_novelty) {
					current_novelty = min_novelty;
				}
			}

			if ( current_novelty < ( _varnames.size() + 1 ) )
				_counts[current_novelty]++;
			else
				_counts[0]++;
			return current_novelty;
		}

		virtual void		reset() {
			_tables.resize( BaseClass::max_novelty() + 1 );
			_counts.resize( BaseClass::max_novelty() + 1 );
			for ( unsigned k = 0; k <= BaseClass::max_novelty(); k++ )
				_counts[k] = 0;
		}

		unsigned	get_num_states( unsigned novelty ) { return _counts[novelty]; }

	protected:

		using Width1Tuple = std::pair<unsigned, int>;
		
		std::unordered_set<Width1Tuple, boost::hash<Width1Tuple>> _width_1_tuples;
		NoveltyTables			_tables;
		std::vector< VariableIndex >	_varnames;
		std::vector< ValueIndex >	_valuation;
		std::vector< unsigned >		_counts;
	};
}
