pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        slackSend(color: '#FFFF00', message: "STARTED: Job '${env.JOB_NAME} [${env.BUILD_NUMBER}]' (${env.BUILD_URL})")
        echo 'Pulling...'+env.BRANCH_NAME
        checkout scm
        echo 'Running CppCheck'
        sh '''cppcheck --enable=warning,performance,portability --inconclusive --xml --xml-version=2 --std=c++11 -iapp/backward -iapp/pugixml -iapp/netcdf4 app 2> cppcheck-result.xml
'''
        publishCppcheck(displayAllErrors: true, displayErrorSeverity: true, displayPortabilitySeverity: true, displayPerformanceSeverity: true, displayWarningSeverity: true, XSize: 500, YSize: 500, displayNoCategorySeverity: true, numBuildsInGraph: 10, severityError: true, threshold: '10', unHealthy: '5', displayStyleSeverity: true, severityWarning: true, severityStyle: true, severityPortability: true, severityNoCategory: true, severityInformation: true, severityPerformance: true)
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