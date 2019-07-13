
#pragma once

#include <cassert>
#include <vector>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <memory>

template <typename T, unsigned int w>
class VEBtree
{
    static constexpr unsigned int half_w = w >> 1;
    static constexpr T mask = ((T)1 << half_w) - 1;
    static constexpr T INFINITY = -1;

    T min = INFINITY, max = 0;
    std::unique_ptr<VEBtree<T, half_w> > summary;
    std::unordered_map<T, VEBtree<T, half_w> > clusters;

public:
    VEBtree() 
    {
	static_assert((w & (w - 1)) == 0, "w given to VEBtree was not power of 2");
    }

    VEBtree(const T val): min{val}, max{val} {}
    
    inline std::vector<T> traverse() const noexcept
    {
        std::vector<T> result;
        if (min == INFINITY) 
            return result;
        result.push_back(min);
        if (!summary) 
            return result;
        std::vector<T> summary_traversal = summary->traverse();
        for (T cluster : summary_traversal)
        {
            const std::vector<T> cluster_traversal = clusters[cluster].traverse();
            for (T num : cluster_traversal)
                result.push_back((cluster << half_w) | num);
        }
        return result;
    }

    inline void insert(const T val) noexcept
    {
        // Update max
	if (val > max)
	    max = val;
        // If empty: just set to min
	if (min == INFINITY) {
	    min = val;
	    return;
	}
        // If we're equal to min, avoid double insertion
        if (val == min) return;
        // If we're smaller than min, put val into min and insert min into deeper structure
        // Break up our val into two halves
        T cluster, idx;
        if (val < min) {
	    cluster = min >> half_w;
            idx = min & mask;
            min = val;
        } else {
            cluster = val >> half_w; 
            idx = val & mask;
        }
        // If the cluster already exists, insert it recursively into the cluster
	if (clusters.count(cluster))
            clusters[cluster].insert(idx); 
        // Otherwise, create a new cluster, shallow insert idx and insert the cluster number into our summary
	else {
	    clusters[cluster] = VEBtree<T, half_w>(idx);
            if (summary)
	        summary->insert(cluster); // Recursive call
            else
                summary = std::make_unique<VEBtree<T, half_w>>(cluster);
	}
    }

    inline T predecessor(const T val) const noexcept
    {
        // If the val is less than min, it has no predecessor
	if (val <= min)
	    return INFINITY;
        // If the val is greater than max, its predecessor is max
	if (val > max)
	    return max;
        // Break up val into halves
	const T cluster = val >> half_w, idx = val & mask;
        // If the supposed cluster doesn't exist, or idx is <= the cluster's min,
        //     this implies that val has no predecessors in clusters[cluster].
        //     We have to find the next closest cluster to its left and find its max.
        //     If this max doesn't exist (ie new_cluster == INFINITY), then
        //     the next smallest number is min.
	if (clusters.count(cluster) == 0 || idx <= clusters.at(cluster).getMin()) {
            if (!summary)
                return (min < val) ? min : INFINITY;
	    const T new_cluster = summary->predecessor(cluster);
            if (new_cluster == INFINITY)
                return (min < val) ? min : INFINITY;
	    return (new_cluster << half_w) | clusters.at(new_cluster).getMax();
        // Otherwise, the predecessor is in the cluster, so just recursively find it
	} else {
	    return (cluster << half_w) | clusters.at(cluster).predecessor(idx);
        }
    }

    inline bool empty() const noexcept
    { return min == INFINITY; }

    inline T getMin() const noexcept
    { return min; }

    inline T getMax() const noexcept
    { return max; }
};

// Base case implementation
template <typename T>
class VEBtree<T, 1>
{
    static constexpr T INFINITY = -1;
    bool hasZero = 0, hasOne = 0;

public:
    VEBtree() {}

    VEBtree(T val): hasZero{!val}, hasOne{!!val} {}

    std::vector<T> traverse() const noexcept
    { return (hasZero && hasOne) ? std::vector<T>{0, 1} : (hasZero ? std::vector<T>{0} : std::vector<T>{1}); }

    void insert(const T val) noexcept
    { val ? (hasOne = 1) : (hasZero = 1); }

    T predecessor(const T val) const noexcept
    { return (val && hasZero) ? 0 : INFINITY; }

    bool empty() const noexcept
    { return !hasZero && !hasOne; }

    T getMin() const noexcept
    { return hasZero ? 0 : (hasOne ? 1 : INFINITY); }

    T getMax() const noexcept
    { return hasOne; }
};
