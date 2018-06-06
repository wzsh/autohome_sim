<template>
    <el-row>
      <el-row class="item-row">
      <el-col :span=3 class="col-title"> LIGHT
      </el-col>
      <el-col :span=21 class="col-comp">
        <el-checkbox-group v-model="ledGroup" @change="toggleLEDs">
          <el-checkbox-button label="R" key="R">RED</el-checkbox-button>
          <el-checkbox-button label="G" key="G">GREEN</el-checkbox-button>
          <el-checkbox-button label="B" key="B">BLUE</el-checkbox-button>
        </el-checkbox-group>
      </el-col>   
      </el-row>
      <el-row class="item-row">
      <el-col :span=6 class="col-title">MUSIC</el-col>
      <el-col :span=18 class="col-comp">
        <el-switch
          v-model="music"
          @change="toggleMusic"
          active-text="On"
          inactive-text="Off">
        </el-switch>
      </el-col>   
      </el-row>
      <el-row class="item-row">
      <el-col :span=6 class="col-title">CURTAIN</el-col>
      <el-col :span=18>
        <el-slider class="slider"
          :max=6
          @change="sendServo"
          v-model="servo">
        </el-slider>
      </el-col>   
      </el-row>
      <el-row class="item-row">
      <el-row>
        <el-col :span=6 class="col-title">MESSAGE</el-col>
        <el-col :span=18></el-col>
      </el-row>
      <el-row>
        <el-input
          type="textarea"
          autosize
          clearable
          placeholder="Message"
          :maxlength=49
          @keyup.enter.native="sendMessage"
          v-model="message" />
      </el-row>   
      </el-row>
    </el-row>

</template>

<script>
import {db} from './Home.vue'

export default {
  name: 'Portal',
  components: {
  },
  data () {
    return {
      messageRef: {},
      ledRef: {},
      musicRef: {},
      servoRef: {},
      ledGroup: [],
      music: false,
      message: '',
      servo: 0
    }
  },
  watch: {
    messageRef: {
      deep: true,
      handler (newObj) {
        if (newObj['.value'] !== undefined) {
          this.message = newObj['.value']
        }
      }
    },
    ledRef: {
      deep: true,
      handler (newObj) {
        if (newObj['.value'] !== undefined) {
          this.ledGroup = newObj['.value'].split('')
        }
      }
    },
    musicRef: {
      deep: true,
      handler (newObj) {
        if (newObj['.value'] !== undefined) {
          this.music = (newObj['.value'] !== 0)
        }
      }
    },
    servoRef: {
      deep: true,
      handler (newObj) {
        if (newObj['.value'] !== undefined) {
          this.servo = newObj['.value']
        }
      }
    }
  },
  created: function () {
  },
  mounted: function () {
    this.$bindAsObject('messageRef', db.ref('message'))
    this.$bindAsObject('musicRef', db.ref('music'))
    this.$bindAsObject('ledRef', db.ref('led'))
    this.$bindAsObject('servoRef', db.ref('servo'))
  },
  methods: {
    sendMessage: function () {
      this.message = this.message.replace(/\n|\r/g, '')
      db.ref('message').set(this.message)
    },
    toggleMusic: function () {
      db.ref('music').set(this.music ? 1 : 0)
    },
    toggleLEDs: function () {
      db.ref('led').set(this.ledGroup.join(''))
    },
    sendServo: function () {
      db.ref('servo').set(this.servo)
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
  padding-top:10px;
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
</style>
