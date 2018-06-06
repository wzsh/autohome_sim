<template>
  <el-container>
  <el-row>
  <el-row class="title">
    Home Automation Simulator
  </el-row>
  <el-tabs v-model="activeName" type="card">
    <el-tab-pane label="Portal" name="portal"><Portal /></el-tab-pane>
    <el-tab-pane label="PID" name="pid"><PID /></el-tab-pane>
  </el-tabs>
  </el-row>
  </el-container>
</template>

<script>
import Portal from './Portal.vue'
import PID from './PID.vue'
import firebase from 'firebase'

let config = {
  // <your firebase config>
}
let fbApp = firebase.initializeApp(config)
export const db = fbApp.database()

export default {
  name: 'Home',
  db: db,
  components: {
    Portal, PID
  },
  data () {
    return {
      activeName: 'portal'
    }
  },
  methods: {
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.el-row{
    width: 100%;
}
.title{
    font-size:28px;
    padding-bottom: 15px;
}
.el-input{
    margin:12px 0;
}
.el-tabs{
    width:100%;
    margin-top: 12px ;
}
</style>
