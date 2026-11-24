// PDC Lab - CI/CD pipeline: GitHub push -> webhook -> Jenkins on AWS EC2
// Asjad Saleh (FA23-BCS-020)

pipeline {
    agent any

    options {
        skipDefaultCheckout(true)   // we check out ourselves in the Checkout stage
        buildDiscarder(logRotator(numToKeepStr: '20'))
    }

    stages {

        stage('Checkout') {
            steps {
                script {
                    // Download the code from GitHub and keep the Git details
                    def scmVars = checkout scm
                    env.GIT_COMMIT = scmVars.GIT_COMMIT
                    env.PREV_COMMIT = scmVars.GIT_PREVIOUS_SUCCESSFUL_COMMIT ?: ''

                    // Who triggered this build (GitHub username of the pusher)
                    def push = currentBuild.getBuildCauses('com.cloudbees.jenkins.GitHubPushCause')
                    def cause = push ? push[0].shortDescription : currentBuild.getBuildCauses()[0].shortDescription
                    echo "TRIGGER : ${cause}"
                }
                sh '''
                    echo "COMMIT  : $(git log -1 --pretty='%h  %s')"
                    echo "AUTHOR  : $(git log -1 --pretty='%an <%ae>')"
                    echo "DATE    : $(git log -1 --pretty='%cd')"
                    echo "CHANGED FILES (A = added, M = modified, D = deleted):"
                    if [ -n "$PREV_COMMIT" ] && git cat-file -e "$PREV_COMMIT^{commit}" 2>/dev/null; then
                        git diff --name-status "$PREV_COMMIT" "$GIT_COMMIT"
                    else
                        git diff-tree --root -r --no-commit-id --name-status "$GIT_COMMIT"
                    fi
                '''
            }
        }

        stage('Build') {
            steps {
                sh 'gcc --version | head -1'
                sh 'gcc -fopenmp -O2 -Wall -o parallel_sum parallel_sum.c -lm'
                sh 'ls -l parallel_sum'
            }
        }

        stage('Test') {
            steps {
                // Run with 1, 2 and 4 threads. Each run must print RESULT: PASS,
                // otherwise grep fails and the stage turns red.
                sh '''
                    for t in 1 2 4; do
                        echo "---- Test with $t thread(s) ----"
                        OMP_NUM_THREADS=$t ./parallel_sum | tee "test_${t}_threads.txt"
                        grep -q "RESULT: PASS" "test_${t}_threads.txt"
                    done
                '''
            }
        }

        stage('Run') {
            steps {
                // Final run using every CPU core of the EC2 instance
                sh 'OMP_NUM_THREADS=$(nproc) ./parallel_sum | tee run_output.txt'
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'test_*_threads.txt, run_output.txt', allowEmptyArchive: true
        }
        success {
            echo 'PIPELINE PASSED: code checked out, built, tested and run on AWS EC2.'
        }
        failure {
            echo 'PIPELINE FAILED: check the red stage above.'
        }
    }
}
