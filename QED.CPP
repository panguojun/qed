/**********************************************************************************		 
			light path simulation
**********************************************************************************/
namespace QEDLP
{
	int T = 100;
	vec2 getpathphase(int pathlength)
	{
		real ang = (real)(pathlength % T) / T * 2 * PI;
		return vec2(cos(ang), sin(ang));
	}
	point_t end;
	int pathcnt = 0;

	// ---------------------------------------------------------------------
	// 费曼的路径积分法模拟。 但是实际情况是计算量太大无法验证，只有在相当简化的模型
	// 里做原理性验证！
	// ---------------------------------------------------------------------
	int maxdepth = 1000;
	vec2 sumphs;
	int mindepth = 1e5;
	vec2 mindepthphs;
	std::map<int, vec2> depthmap;
	bool bupdown = 1;
	bool lightpath(int ix, int iy, int depth = 0)
	{
		if (iy == end.y)
		{
			vec2 phs = getpathphase(depth);
			sumphs = sumphs + phs;
			
			if (depthmap.find(depth) == depthmap.end())
				depthmap[depth] = phs;
			depthmap[depth] += phs;

			return true;
		}

		if (depth > maxdepth)
		{
			//PRINT("depth OUT!");
			return false;
		}

		if (ix < 0 || ix > 1024 - 1 || iy < 0 || iy > 1024 - 1)
		{
			return false;
		}

		{
			int dir = rand() % 5;
			bool ret = 0;
			switch (dir)
			{
			case 0:
				ret = lightpath(bupdown == 1 ? ix + 1 : ix - 1, iy, depth + 1);
				break;
			default:
				ret = lightpath(ix, iy + 1, depth + 1);
				break;
			}

			if (ret)
			{
				pixel(ix, iy, lp::pathcnt % 2 == 0 ? 0xFFFFFFFF : 0xFF0000FF);
			}
			return ret;
		}
	}
	// ---------------------------------------------------------------------
	// 反射路径模拟
	// ---------------------------------------------------------------------
	const int MAX_STEP = 10000;
	const int STEPS = 1000;
	const int step = 1;
	const real MIN_ENAGEY = 0.01;
	const real n1 = 1;
	const real n2 = 2;
	void refpath(vec2 p, vec2 v, real eg, int phs, int numsteps)
	{
		for (int i = 0; i < STEPS; i++)
		{
			if (eg < MIN_ENAGEY)
				return;

			vec2 np = p + v * step;
			phs += step;
			numsteps += step;

			pixel((int)np.x, (int)np.y, blendcor(0xFF101010, 0xFF00FFFF, eg / 100.0f));
			if (numsteps > MAX_STEP)
			{
				return;
			}
			if (p.y >= 250 && np.y < 250)
			{
				real neg = eg * (n1 / (n1 + n2));
				refpath(p, vec2(v.x, -v.y), neg, -phs, numsteps);
				eg -= neg;
				v.x *= (n1 / n2);
				v.norm();
			}
			if (p.y <= 250 && np.y > 250)
			{
				real neg = eg * (n2 / (n1 + n2));
				refpath(p, vec2(v.x, -v.y), neg, -phs, numsteps);
				eg -= neg;
				v.x *= (n2 / n1);
				v.norm();
			}
			if (p.y >= 200 && np.y < 200)
			{
				real neg = eg * (n1 / (n1 + n2));
				refpath(p, vec2(v.x, -v.y), neg, phs, numsteps);
				eg -= neg;
				v.x *= (n1 / n2);
				v.norm();
			}
			p = np;
		}
	}
}
