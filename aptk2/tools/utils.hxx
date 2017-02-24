
#pragma once


namespace lapkt { namespace utils {

//! Compute intersection and set differences at the same time
template <typename T>
void intersection_and_set_diff(const std::vector<T>& A, const std::vector<T>& B,
                               std::vector<T>& inters, std::vector<T>& AminusB, std::vector<T>& BminusA);

} } // namespaces

