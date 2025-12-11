from sympy import symbols, Eq, solve, exp, log

# Definir símbolos
Linf, Lmax, tmax, k, LB, xmid, L, t, t0, a, b = symbols('Linf Lmax tmax k LB xmid L t t0 a b')

# Definir la ecuación
equation = Eq(Lmax, exp(k*tmax + a) + LB - exp(a))

# Resolver la ecuación para Linf
Linf_solution_step_by_step = solve(equation, a, manual=True)

# Mostrar la solución paso a paso
for step in Linf_solution_step_by_step:
    print(step)



# b = LB - exp(a)

# a = ln(-(LB-Lmax)/(exp(k*tmax) - 1))
