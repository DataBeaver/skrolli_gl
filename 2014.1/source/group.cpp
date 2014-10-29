#include <algorithm>
#include <fstream>
#include <sstream>
#include "group.h"
#include "instance.h"
#include "object.h"

using namespace std;

namespace SkrolliGL {

Group::~Group()
{
	for(vector<Instance *>::iterator i=instances.begin(); i!=instances.end(); ++i)
		delete *i;
}

void Group::add(const Renderable &r)
{
	contents.push_back(&r);
}

void Group::remove(const Renderable &r)
{
	contents.erase(std::remove(contents.begin(), contents.end(), &r), contents.end());
}

void Group::load(const ResourceManager &res_mgr, const string &filename)
{
	ifstream input(filename.c_str());
	string line;

	Instance *current = 0;
	while(getline(input, line))
	{
		if(line[0]=='#')
			continue;

		istringstream parse(line);
		string command;
		parse >> command;

		if(command=="object")
		{
			string name;
			parse >> name;
			current = new Instance(res_mgr.get<Object>(name+".obj"));
			instances.push_back(current);
			add(*current);
		}
		else if(current)
		{
			if(command=="translate")
			{
				float x, y, z;
				parse >> x >> y >> z;
				current->set_matrix(current->get_matrix()*Matrix::translation(x, y, z));
			}
			else if(command=="rotate_x")
			{
				float angle;
				parse >> angle;
				current->set_matrix(current->get_matrix()*Matrix::rotation_x(angle));
			}
			else if(command=="rotate_y")
			{
				float angle;
				parse >> angle;
				current->set_matrix(current->get_matrix()*Matrix::rotation_y(angle));
			}
			else if(command=="rotate_z")
			{
				float angle;
				parse >> angle;
				current->set_matrix(current->get_matrix()*Matrix::rotation_z(angle));
			}
		}
	}
}

void Group::render(const RenderState &state) const
{
	for(vector<const Renderable *>::const_iterator i=contents.begin(); i!=contents.end(); ++i)
		(*i)->render(state);
}

} // namespace SkrolliGL
