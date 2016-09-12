#pragma once

void clearFluidBox() {
	for (int i = 0; i < fluidBoxArraySize; i++) {
		u[i] = 0.0f;
		v[i] = 0.0f;
		u_prev[i] = 0.0f;
		v_prev[i] = 0.0f;
		dens[i] = 0.05f;
		dens_prev[i] = 0.0f;
		fluidObstacles[i] = false;
	}
}

void add_source(float * x, float * s, float dt) {
	for (int i = 0; i<fluidBoxArraySize; i++)
		x[i] += dt*s[i];
}

void set_bnd(int b, float * x) {
	for (int i = 1; i <= fluidBoxX; i++) {
		for (int j = 1; j <= fluidBoxY; j++) {
			if (fluidObstacles[IX(i, j)]) {

				x[IX(i, j)] = x[IX(i - 1, j)];
				x[IX(i, j)] -= x[IX(i, j - 1)];
				x[IX(i, j)] += x[IX(i + 1, j)];
				x[IX(i, j)] -= x[IX(i + 1, j + 1)];

				x[IX(i, j)] *= 0.25;
			}
		}
	}
	
	for (int i = 1; i <= fluidBoxY; i++) {
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(fluidBoxX + 1, i)] = b == 1 ? -x[IX(fluidBoxX, i)] : x[IX(fluidBoxX, i)];
	}
	for (int i = 1; i <= fluidBoxX; i++) {
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, fluidBoxY + 1)] = b == 2 ? -x[IX(i, fluidBoxY)] : x[IX(i, fluidBoxY)];
	}

	

	x[IX(0, 0)] = 0.5f*(x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, fluidBoxY + 1)] = 0.5f*(x[IX(1, fluidBoxY + 1)] + x[IX(0, fluidBoxY)]);
	x[IX(fluidBoxX + 1, 0)] = 0.5f*(x[IX(fluidBoxX, 0)] + x[IX(fluidBoxX + 1, 1)]);
	x[IX(fluidBoxX + 1, fluidBoxY + 1)] = 0.5f*(x[IX(fluidBoxX, fluidBoxY + 1)] + x[IX(fluidBoxX + 1, fluidBoxY)]);

}

void project(float * u, float * v, float * p, float * div) {
	float hx = 1.0f / fluidBoxX;
	float hy = 1.0f / fluidBoxY;
	for (int i = 1; i <= fluidBoxX; i++) {
		for (int j = 1; j <= fluidBoxY; j++) {
			div[IX(i, j)] = -0.5f * hy * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			p[IX(i, j)] = 0;
		}
	}
	set_bnd(0, div);
	set_bnd(0, p);
	for (int k = 0; k<20; k++) {
		for (int i = 1; i <= fluidBoxX; i++) {
			for (int j = 1; j <= fluidBoxY; j++) {
				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
			}
		}
		set_bnd(0, p);
	}
	for (int i = 1; i <= fluidBoxX; i++) {
		for (int j = 1; j <= fluidBoxY; j++) {
			u[IX(i, j)] -= 0.5f *(p[IX(i + 1, j)] - p[IX(i - 1, j)]) / hx;
			v[IX(i, j)] -= 0.5f *(p[IX(i, j + 1)] - p[IX(i, j - 1)]) / hy;
		}
	}
	set_bnd(1, u);
	set_bnd(2, v);
}

void diffuse(int b, float * x, float * x0, float diff, float dt) {
	float a = dt*diff*fluidBoxX*fluidBoxY;
	for (int k = 0; k<20; k++) {
		for (int i = 1; i <= fluidBoxX; i++) {
			for (int j = 1; j <= fluidBoxY; j++) {
				x[IX(i, j)] = (x0[IX(i, j)] + a*(x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / (1 + 4 * a);
			}
		}
		set_bnd(b, x);
	}
}

void advect(int b, float * d, float * d0, float * u, float * v, float dt) {
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0x, dt0y;
	dt0x = dt * fluidBoxX;
	dt0y = dt * fluidBoxY;
	for (i = 1; i <= fluidBoxX; i++) {
		for (j = 1; j <= fluidBoxY; j++) {
			x = i - dt0x*u[IX(i, j)];
			y = j - dt0y*v[IX(i, j)];
			if (x<0.5)
				x = 0.5;
			if (x>fluidBoxX + 0.5)
				x = fluidBoxX + 0.5f; i0 = (int)x;
			i1 = i0 + 1;
			if (y<0.5)
				y = 0.5f;
			if (y>fluidBoxY + 0.5)
				y = fluidBoxY + 0.5f;
			j0 = (int)y;
			j1 = j0 + 1;
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;
			d[IX(i, j)] = s0*(t0*d0[IX(i0, j0)] + t1*d0[IX(i0, j1)]) + s1*(t0*d0[IX(i1, j0)] + t1*d0[IX(i1, j1)]);
		}
	}
	set_bnd(b, d);
}

void dens_step(float * x, float * x0, float * u, float * v, float diff, float dt) {
	add_source(x, x0, dt);
	float *tmp = x0;
	x0 = x;
	x = tmp;
	diffuse(0, x, x0, diff, dt);
	tmp = x0;
	x0 = x;
	x = tmp;
	advect(0, x, x0, u, v, dt);
}

void vel_step(float * u, float * v, float * u0, float * v0, float visc, float dt) {
	add_source(u, u0, dt);
	add_source(v, v0, dt);
	float *tmp = u0; u0 = u; u = tmp;
	diffuse(1, u, u0, visc, dt);
	tmp = v0; v0 = v; v = tmp;
	diffuse(2, v, v0, visc, dt);
	project(u, v, u0, v0);
	tmp = u0; u0 = u; u = tmp;
	tmp = v0; v0 = v; v = tmp;
	advect(1, u, u0, u0, v0, dt);
	advect(2, v, v0, u0, v0, dt);
	project(u, v, u0, v0);
}
