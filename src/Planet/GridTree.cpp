#include "Planet/GridTree.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

namespace Planet
{
	std::size_t GridTree::countLeaves() const {
		std::size_t accumulator = 0;
		if (!nodes.empty()) {
			for (auto &node : nodes) {
				accumulator += node.countLeaves();
			}
		} else {
			++accumulator;
		}
		return accumulator;
	}

	void GridTree::draw(const Grid &grid, const std::function<void(const glm::mat4&)> &set_model) const {
		if (!nodes.empty()) {
			for (auto &node : nodes) {
				node.draw(grid, set_model);
			}
		} else {
			// The grid has a size of [2, 2], so the scale can remain the half_size
			glm::vec3 scale = {half_size.x, half_size.y, half_size.y};

			// Upload uniform and draw
			set_model(glm::scale(glm::translate(glm::mat4(1.f), glm::vec3{origin, 0.0}), scale));
			grid.draw();
		}
	}

	GridTree::GridTree(int max_depth, int depth, const glm::vec2 &position, const glm::vec2 &origin, const glm::vec2 &half_size, const GridTree *parent) :
		origin(origin),
		half_size(half_size),
		parent(parent)
	{
		// Check whether the position is within half_size distance of the node's center or four corners
		// This is much simpler to implement than real square-to-sphere collision testing
		bool collision = false;
		if (glm::length2(position - origin) < glm::length2(half_size)) {
			collision = true;
		} else if(glm::length2(position - origin + half_size*glm::vec2{ 1.f, 1.f}) < glm::length2(half_size)) {
			collision = true;
		} else if(glm::length2(position - origin + half_size*glm::vec2{ 1.f,-1.f}) < glm::length2(half_size)) {
			collision = true;
		} else if(glm::length2(position - origin + half_size*glm::vec2{-1.f, 1.f}) < glm::length2(half_size)) {
			collision = true;
		} else if(glm::length2(position - origin + half_size*glm::vec2{-1.f,-1.f}) < glm::length2(half_size)) {
			collision = true;
		}

		// If there's a collision, subdivide the tree
		if (depth < max_depth && collision) {
			nodes = std::vector<GridTree>{
				{max_depth, depth+1, position, origin + half_size*glm::vec2{ 0.5f, 0.5f}, half_size*0.5f, this},
				{max_depth, depth+1, position, origin + half_size*glm::vec2{ 0.5f,-0.5f}, half_size*0.5f, this},
				{max_depth, depth+1, position, origin + half_size*glm::vec2{-0.5f, 0.5f}, half_size*0.5f, this},
				{max_depth, depth+1, position, origin + half_size*glm::vec2{-0.5f,-0.5f}, half_size*0.5f, this}
			};
		}
	}
}
