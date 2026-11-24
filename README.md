# PDC Lab: CI/CD with GitHub, Jenkins and AWS EC2

**Asjad Saleh (FA23-BCS-020)**, Parallel and Distributed Computing Lab

Every push to this repository sends a GitHub webhook to Jenkins running on an AWS EC2 instance.
Jenkins then runs the pipeline in `Jenkinsfile`:

1. **Checkout**: pulls the latest code and prints who pushed and which files changed
2. **Build**: compiles `parallel_sum.c` with `gcc -fopenmp`
3. **Test**: runs the program with 1, 2 and 4 OpenMP threads and checks every run prints `RESULT: PASS`
4. **Run**: runs the program using all CPU cores of the EC2 instance

`parallel_sum.c` computes the sum 1..100,000,000 and the value of PI in parallel with OpenMP
`reduction(+)`, and checks both against the exact answers.
