<template>
  <el-row>
    <el-row class="item-row">
    <el-col :span=10 class="col-title"> Temperature </el-col>
    <el-col :span=14 class="col-comp">
      <el-input
        v-model="temperature"
        :disabled="true">
      </el-input>
    </el-col>   
    </el-row>

    <el-row class="item-row">
      <el-row>
        <el-col :span=10 class="col-title"> Heater's Target Temperature </el-col>
        <el-col :span=14 class="col-comp">
          <el-input
            v-model.number="target_temper"
            :max=70
            :min=0
          >
          </el-input>
        </el-col>   
      </el-row>
      <el-row>
        <el-col :span=18 class="col-title">
        <el-slider class="slider"
          :max=70
          v-model.number="target_temper">
        </el-slider>
        </el-col>
        <el-col :span=6 v-show="pid_stopped"> 
          <el-button type="primary" v-on:click="setTargetTemper" >SET</el-button> 
        </el-col>
        <el-col :span=6 v-show="!pid_stopped"> 
          <el-button type="danger" v-on:click="stopTargetTemper" v-show="!pid_stopped">STOP</el-button> 
        </el-col>
      </el-row>
    </el-row>

    <el-row class="item-row">
      <el-row>
        <el-col :span=6 class="col-title">GRPAH</el-col>
        <el-col :span=18></el-col>
      </el-row>
      <el-row>
        <vue-highcharts :options="options" ref="lineCharts"></vue-highcharts>
      </el-row>   
      </el-row>

  </el-row>
  
</template>

<script>
import VueHighcharts from 'vue2-highcharts'
import {db} from './Home.vue'

export default {
  name: 'PID',
  components: {
    VueHighcharts
  },
  data () {
    return {
      temperRef: {},
      targetTemperRef: {},
      pidRef: [],
      temperature: 0.0,
      target_temper: 0.0,
      pid_stopped: true,
      targetLine: {
        name: 'Target Temper',
        color: '#ff0000',
        marker: {
          enabled: false
        },
        data: []
      },
      asyncData: {
        name: 'Heater',
        marker: {
          symbol: 'square'
        },
        data: []
      },
      options: {
        chart: {
          type: 'spline'
        },
        title: {
          text: 'PID Temperature'
        },
        xAxis: {
          softMax: 80,
          categories: []
        },
        yAxis: {
          softMax: 80,
          title: {
            text: 'Temperature'
          },
          labels: {
            formatter: function () {
              return this.value + '°'
            }
          }
        },
        tooltip: {
          crosshairs: true,
          shared: true
        },
        credits: {
          enabled: false
        },
        plotOptions: {
          spline: {
            marker: {
              radius: 2,
              lineColor: '#666666',
              lineWidth: 1
            }
          }
        },
        series: []
      }
    }
  },
  watch: {
    pidRef: {
      deep: true,
      handler (newArray) {
        if (this.pid_stopped) return
        for (let key in newArray) {
          let value = newArray[key]
          let temp = value.split(':')
          let index = parseInt(temp[0])
          let temper = parseFloat(temp[1])
          this.asyncData.data[index] = temper
          this.targetLine.data[index] = this.target_temper
        }
        // console.log(this.asyncData.data)
        // let xAxisLength = (Math.round(this.asyncData.data.length / 50) + 1) * 50
        // this.options.xAxis.softMax = xAxisLength
        let lineCharts = this.$refs.lineCharts
        lineCharts.getChart().update(this.options)
      }
    },
    temperRef: {
      deep: true,
      handler (newObj) {
        if (newObj['.value'] !== undefined) {
          this.temperature = newObj['.value'].toFixed(2)
        }
      }
    },
    targetTemperRef: {
      deep: true,
      handler (newObj) {
        if (newObj['.value'] !== undefined) {
          this.target_temper = newObj['.value']
        }
      }
    }
  },
  mounted: function () {
    this.$bindAsObject('temperRef', db.ref('temp'))
    this.$bindAsObject('targetTemperRef', db.ref('target_temp'))
    this.$bindAsObject('pidRef', db.ref('pid'))
  },
  methods: {
    setTargetTemper (e) {
      this.pid_stopped = false
      db.ref('target_temp').set(this.target_temper)
      let lineCharts = this.$refs.lineCharts
      lineCharts.removeSeries()
      lineCharts.addSeries(this.targetLine)
      lineCharts.addSeries(this.asyncData)
    },
    stopTargetTemper (e) {
      this.pid_stopped = true
      this.target_temper = 0
      db.ref('target_temp').set(this.target_temper)
      this.pidRef = []
      this.asyncData.data = []
      this.options.xAxis.categories = []
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.item-row{
  padding-top: 10px; 
  padding-bottom: 15px;
}
.col-title{
  padding-top: 15px;
  padding-bottom:10px;
  text-align: left;
}
.col-comp{
  text-align: right;
}
.el-row.content{
    padding:16px;
}
.slider{
  text-align: right;
  padding-left:30px;
  padding-right:12px;
}
.title{
    font-size:28px;
}
.el-input{
    width: 80px;
}
.el-button{
    width:100%;
    margin-top: 12px ;
}
</style>
