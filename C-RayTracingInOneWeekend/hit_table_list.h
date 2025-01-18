#pragma once

#include "hit_table.h"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class hit_table_list : hit_table
{
public:
	std::vector<shared_ptr<hit_table>> objects;

	hit_table_list();
	~hit_table_list();

	hit_table_list(shared_ptr<hit_table> obj);

	void add(shared_ptr<hit_table> object);
	void clear();

	bool hit(ray& r, double ray_t_min, double ray_t_max, hit_record& rec) const override;

};

hit_table_list::hit_table_list() = default;
hit_table_list::~hit_table_list() = default;

hit_table_list::hit_table_list(shared_ptr<hit_table> obj)
{
	this->add(obj);
}

void hit_table_list::add(shared_ptr<hit_table> object)
{
	this->objects.push_back(object);
}

void hit_table_list::clear()
{
	this->objects.clear();
}

bool hit_table_list::hit(ray& r, double ray_t_min, double ray_t_max, hit_record& rec) const
{
	hit_record tem_rec;
	bool hit_any = false;
	auto closest_so_far = ray_t_max;

	for (const auto& obj : this->objects)
	{
		if (obj->hit(r, ray_t_min, closest_so_far, tem_rec))
		{
			hit_any = true;
			closest_so_far = tem_rec.t;
			rec = tem_rec;
		}
	}

	return hit_any;
}
