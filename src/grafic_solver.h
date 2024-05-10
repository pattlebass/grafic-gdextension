#ifndef GRAFIC_SOLVER_H
#define GRAFIC_SOLVER_H

#include <godot_cpp/classes/resource.hpp>

namespace godot {
class GraficSolver : public Resource {
	GDCLASS(GraficSolver, Resource)

	int zile_luna;
	int zile_100;
	int numar_persoane;
	PackedInt32Array norme;
	PackedInt32Array ore_noapte;
	PackedInt32Array co;
	PackedInt32Array ore_100;

	int ture_posibile[15][1000000];
	int nr_ture_posibile[15] = { 0 };
	int nr_solutii_cautate = 1;
	int nr_solutii_gasite = 0;
	const int MAX_SOLUTII_BAZA = 1;
	int solutii_baza_curr = 0;
	Array solutii_gasite;

protected:
	static void _bind_methods();

public:
	GraficSolver();
	~GraficSolver();

	Error load_ture_posibile();

	bool ok(int v[], int k, int n);

	bool solutie(int k, int n);

	void salveaza_solutia(int v[], int n);

	void back(int v[], int k, int n);

	void solve(int zile_luna,
			int zile_100,
			int numar_persoane,
			PackedInt32Array norme,
			PackedInt32Array ore_noapte,
			PackedInt32Array co,
			PackedInt32Array ore_100);

	Array get_solutii_gasite();

	void set_nr_solutii_cautate(int p_nr_solutii_cautate);
	int get_nr_solutii_cautate();
};

} //namespace godot

#endif