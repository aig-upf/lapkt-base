
/*
Lightweight Automated Planning Toolkit
Copyright (C) 2015

<contributors>
Miquel Ramirez <miquel.ramirez@rmit.edu.au>
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

#include <lapkt/search/interfaces/open_list.hxx>
#include <deque>
#include <cassert>

namespace aptk {
	
	template < typename NodeType >
	class StlUnsortedLIFO : public OpenList< NodeType, std::deque< std::shared_ptr<NodeType> > > {
	public:

		typedef std::shared_ptr< NodeType >	NodePtrType;

		virtual ~StlUnsortedLIFO() { }

		virtual	void insert( NodePtrType n ) override {
			this->push_back( n );
		}

		virtual NodePtrType get_next( ) override {
			assert( !empty() );
			return this->pop_back();
		}

		virtual bool empty() const override { 
			return this->empty();
		}
	};

}
