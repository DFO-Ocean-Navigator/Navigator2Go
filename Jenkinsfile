pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        slackSend(color: '#FFFF00', message: "STARTED: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]' (${env.BUILD_URL})")
        echo 'Pulling...'+env.BRANCH_NAME
        checkout scm
        echo 'Running CppCheck'
        sh '''cppcheck -j 12 --enable=all --inconclusive --xml --xml-version=2 --std=c++11 -iapp/backward -iapp/pugixml -iapp/netcdf4 app 2> cppcheck.xml
'''
        publishCppcheck(displayAllErrors: true, displayErrorSeverity: true, displayStyleSeverity: true, displayPortabilitySeverity: true, displayPerformanceSeverity: true, displayWarningSeverity: true, XSize: 500, YSize: 200, displayNoCategorySeverity: true)
      }
    }
  }
  post {
    success {
      slackSend(color: '#00FF00', message: "SUCCESSFUL: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]' (${env.BUILD_URL})")

    }

    failure {
      slackSend(color: '#FF0000', message: "FAILED: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]' (${env.BUILD_URL})")

    }

  }
}